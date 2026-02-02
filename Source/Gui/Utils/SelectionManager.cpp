#include "SelectionManager.h"

#include <algorithm>
#include <cmath>

#include "Gui/Utils/CursorController.h"
#include "Gui/Utils/ViewRangeMapper.h"

SelectionManager::SelectionManager(Edit& edit)
    : edit(edit) {
}

void SelectionManager::setCursorController(CursorController* controller) {
    cursorController = controller;
}

void SelectionManager::setSelection(const std::vector<String>& ids) {
    std::unordered_set<String> next;
    next.reserve(ids.size());
    for (const auto& id : ids) {
        next.insert(id);
    }

    std::vector<String> added;
    std::vector<String> removed;

    for (const auto& id : next) {
        if (selectedIds.find(id) == selectedIds.end()) {
            added.push_back(id);
        }
    }

    for (const auto& id : selectedIds) {
        if (next.find(id) == next.end()) {
            removed.push_back(id);
        }
    }

    selectedIds = std::move(next);
    if (!added.empty() || !removed.empty()) {
        notifyListeners();
    }
}

bool SelectionManager::isSelected(const String& trackId) const {
    return selectedIds.find(trackId) != selectedIds.end();
}

void SelectionManager::addListener(Listener* listener) {
    listeners.add(listener);
}

void SelectionManager::removeListener(Listener* listener) {
    listeners.remove(listener);
}

std::optional<int64_t> SelectionManager::getSelectionAnchorSample() const {
    if (edit.getState().hasSelectionRange()) {
        return edit.getState().getSelectionStartSample();
    }
    return std::nullopt;
}

std::optional<std::pair<int64_t, int64_t>> SelectionManager::getSelectionRangeSamples() const {
    if (!edit.getState().hasSelectionRange()) {
        return std::nullopt;
    }
    return std::make_pair(edit.getState().getSelectionStartSample(),
                          edit.getState().getSelectionEndSample());
}

bool SelectionManager::isSelectingRange() const {
    return isSelecting;
}

void SelectionManager::collapseSelectionToSample(int64 sample) {
    if (!selectionAnchorSample.has_value() || !selectionHoverSample.has_value()) {
        selectionAnchorSample = sample;
        selectionHoverSample = sample;
        edit.getState().setSelectionRange(sample, sample);
        notifyListeners();
        return;
    }
    if (selectionAnchorSample.value() == sample && selectionHoverSample.value() == sample) {
        return;
    }
    selectionAnchorSample = sample;
    selectionHoverSample = sample;
    edit.getState().setSelectionRange(sample, sample);
    notifyListeners();
}

void SelectionManager::mouseDown(const juce::MouseEvent& event, juce::Component* relativeTo) {
    if (!event.mods.isLeftButtonDown()) {
        return;
    }
    auto relative = event.getEventRelativeTo(relativeTo);
    anchorTrackIndex = getTrackIndexAtY(static_cast<int>(relative.position.y));
    if (anchorTrackIndex < 0) {
        isSelecting = false;
        selectionAnchorSample.reset();
        selectionHoverSample.reset();
        if (cursorController != nullptr) {
            cursorController->onSelectionCleared();
        }
        notifyListeners();
        return;
    }
    selectionAnchorSample = getSampleAtPosition(event, relativeTo);
    selectionHoverSample = selectionAnchorSample;
    if (!selectionAnchorSample.has_value()) {
        isSelecting = false;
        if (cursorController != nullptr) {
            cursorController->onSelectionCleared();
        }
        notifyListeners();
        return;
    }
    isSelecting = true;
    hoverTrackIndex = -1;
    updateSelectionRange(anchorTrackIndex);
    updateSelectionSamples();
    notifyListeners();
}

void SelectionManager::mouseDrag(const juce::MouseEvent& event, juce::Component* relativeTo) {
    if (!isSelecting) {
        return;
    }
    const auto sample = getSampleAtPosition(event, relativeTo);
    if (sample.has_value() && sample != selectionHoverSample) {
        selectionHoverSample = sample;
        updateSelectionSamples();
        notifyListeners();
    }
    auto relative = event.getEventRelativeTo(relativeTo);
    const int index = getTrackIndexAtY(static_cast<int>(relative.position.y));
    if (index >= 0) {
        updateSelectionRange(index);
    }
}

void SelectionManager::mouseMove(const juce::MouseEvent& event, juce::Component* relativeTo) {
    if (!isSelecting) {
        return;
    }
    const auto sample = getSampleAtPosition(event, relativeTo);
    if (sample.has_value() && sample != selectionHoverSample) {
        selectionHoverSample = sample;
        updateSelectionSamples();
        notifyListeners();
    }
    auto relative = event.getEventRelativeTo(relativeTo);
    const int index = getTrackIndexAtY(static_cast<int>(relative.position.y));
    if (index >= 0) {
        updateSelectionRange(index);
    }
}

void SelectionManager::mouseEnter(const juce::MouseEvent& event, juce::Component* relativeTo) {
    if (!isSelecting) {
        return;
    }
    const auto sample = getSampleAtPosition(event, relativeTo);
    if (sample.has_value() && sample != selectionHoverSample) {
        selectionHoverSample = sample;
        updateSelectionSamples();
        notifyListeners();
    }
    auto relative = event.getEventRelativeTo(relativeTo);
    const int index = getTrackIndexAtY(static_cast<int>(relative.position.y));
    if (index >= 0) {
        updateSelectionRange(index);
    }
}

void SelectionManager::mouseUp() {
    if (selectionAnchorSample.has_value() && selectionHoverSample.has_value()) {
        const auto rangeStart = std::min(selectionAnchorSample.value(), selectionHoverSample.value());
        if (cursorController != nullptr) {
            cursorController->setCursorSample(rangeStart);
        } else {
            edit.getState().setCursorSample(rangeStart);
            if (auto transport = edit.getTransport()) {
                if (!transport->isPlaying()) {
                    transport->setPlayheadSample(rangeStart);
                }
            }
        }
    } else {
        if (cursorController != nullptr) {
            cursorController->onSelectionCleared();
        }
    }
    isSelecting = false;
    anchorTrackIndex = -1;
    hoverTrackIndex = -1;
    notifyListeners();
}

int SelectionManager::getTrackIndexAtY(int y) const {
    int currentY = edit.getState().getTimelineHeight();
    int index = 0;
    for (const auto& track : edit.getTracks()) {
        if (!track) {
            continue;
        }
        const int height = static_cast<int>(track->getHeight());
        if (y >= currentY && y < currentY + height) {
            return index;
        }
        currentY += height;
        ++index;
    }
    return -1;
}

void SelectionManager::updateSelectionRange(int hoverIndex) {
    if (anchorTrackIndex < 0 || hoverIndex < 0) {
        return;
    }
    if (hoverIndex == hoverTrackIndex) {
        return;
    }
    hoverTrackIndex = hoverIndex;
    const int startIndex = std::min(anchorTrackIndex, hoverTrackIndex);
    const int endIndex = std::max(anchorTrackIndex, hoverTrackIndex);
    std::vector<String> selectedIds;
    selectedIds.reserve(static_cast<size_t>(endIndex - startIndex + 1));
    int index = 0;
    for (const auto& track : edit.getTracks()) {
        if (!track) {
            ++index;
            continue;
        }
        if (index >= startIndex && index <= endIndex) {
            selectedIds.push_back(track->getId());
        }
        ++index;
    }
    setSelection(selectedIds);
}

void SelectionManager::updateSelectionSamples() {
    if (!selectionAnchorSample.has_value() || !selectionHoverSample.has_value()) {
        if (cursorController != nullptr) {
            cursorController->onSelectionCleared();
        }
        return;
    }
    if (cursorController != nullptr) {
        cursorController->onSelectionRangeChanged(selectionAnchorSample.value(), selectionHoverSample.value());
    }
}

void SelectionManager::notifyListeners() {
    listeners.call([](Listener& listener) {
        listener.selectionChanged();
    });
}

std::optional<int64_t> SelectionManager::getSampleAtPosition(const juce::MouseEvent& event,
                                                             juce::Component* relativeTo) const {
    if (relativeTo == nullptr) {
        return std::nullopt;
    }
    const auto mapper = getMapperForComponent(*relativeTo);
    const auto relative = event.getEventRelativeTo(relativeTo);
    return mapper.xToSample(relative.position.x);
}

ViewRangeMapper SelectionManager::getMapperForComponent(const juce::Component& component) const {
    ViewRangeMapper mapper(edit, static_cast<float>(component.getWidth()));
    if (!mapper.isValid()) {
        // View range and width must be valid to map samples.
        jassert(false);
    }
    return mapper;
}
