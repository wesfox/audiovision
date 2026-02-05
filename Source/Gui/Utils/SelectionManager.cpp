#include "SelectionManager.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>

#include "Gui/Utils/CursorController.h"
#include "Gui/Utils/ViewRangeMapper.h"

SelectionManager::SelectionManager(Edit& edit)
    : edit(edit) {
}

void SelectionManager::setCursorController(CursorController* controller) {
    cursorController = controller;
}

CursorController& SelectionManager::getCursorController() {
    if (cursorController == nullptr) {
        // CursorController must be set before using SelectionManager.
        jassert(false);
    }
    return *cursorController;
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

std::vector<String> SelectionManager::getSelectedTrackIds() const {
    std::vector<String> ids;
    ids.reserve(selectedIds.size());
    for (const auto& track : edit.getTracks()) {
        if (!track) {
            continue;
        }
        const auto trackId = track->getId();
        if (selectedIds.find(trackId) != selectedIds.end()) {
            ids.push_back(trackId);
        }
    }
    return ids;
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
        applySelectionSamples(sample, sample);
        notifyListeners();
        return;
    }
    if (selectionAnchorSample.value() == sample && selectionHoverSample.value() == sample) {
        return;
    }
    selectionAnchorSample = sample;
    selectionHoverSample = sample;
    applySelectionSamples(sample, sample);
    notifyListeners();
}

void SelectionManager::setSelectionRangeFromCommand(int64 anchorSample, int64 hoverSample) {
    selectionAnchorSample = anchorSample;
    selectionHoverSample = hoverSample;
    isSelecting = false;
    forceSelectionCommit = false;
    applySelectionSamples(anchorSample, hoverSample);
    notifyListeners();
}

void SelectionManager::mouseDown(const juce::MouseEvent& event, juce::Component* relativeTo) {
    if (!event.mods.isLeftButtonDown()) {
        return;
    }
    forceSelectionCommit = false;
    selectionComponentWidth = relativeTo != nullptr ? relativeTo->getWidth() : 0;
    auto relative = event.getEventRelativeTo(relativeTo);
    const int clickedTrackIndex = getTrackIndexAtY(static_cast<int>(relative.position.y));
    if (clickedTrackIndex < 0) {
        isSelecting = false;
        selectionAnchorSample.reset();
        selectionHoverSample.reset();
        lastAnchorSample.reset();
        lastAnchorTrackIndex = -1;
        getCursorController().onSelectionCleared();
        notifyListeners();
        return;
    }
    const auto clickSample = getSampleAtPosition(event, relativeTo);
    if (event.mods.isShiftDown() && clickSample.has_value()) {
        std::optional<int64_t> anchorSample;
        if (edit.getState().hasSelectionRange()) {
            const auto rangeStart = edit.getState().getSelectionStartSample();
            const auto rangeEnd = edit.getState().getSelectionEndSample();
            if (clickSample.value() >= rangeStart && clickSample.value() <= rangeEnd) {
                const auto centerSample = rangeStart + ((rangeEnd - rangeStart) / 2);
                anchorSample = clickSample.value() < centerSample ? rangeEnd : rangeStart;
            } else {
                const auto distanceToStart = std::llabs(clickSample.value() - rangeStart);
                const auto distanceToEnd = std::llabs(clickSample.value() - rangeEnd);
                anchorSample = distanceToStart >= distanceToEnd ? rangeStart : rangeEnd;
            }
        } else if (lastAnchorSample.has_value()) {
            anchorSample = lastAnchorSample;
        }

        if (anchorSample.has_value()) {
            int minSelectedIndex = -1;
            int maxSelectedIndex = -1;
            int index = 0;
            for (const auto& track : edit.getTracks()) {
                if (!track) {
                    ++index;
                    continue;
                }
                if (selectedIds.find(track->getId()) != selectedIds.end()) {
                    if (minSelectedIndex < 0) {
                        minSelectedIndex = index;
                    }
                    maxSelectedIndex = index;
                }
                ++index;
            }
            const bool hasTrackRange = minSelectedIndex >= 0;
            const bool clickedInsideRange = hasTrackRange
                && clickedTrackIndex >= minSelectedIndex
                && clickedTrackIndex <= maxSelectedIndex;
            const bool isShorteningTrackRange = clickedInsideRange
                && clickedTrackIndex > minSelectedIndex;
            int startIndex = clickedTrackIndex;
            int endIndex = clickedTrackIndex;

            if (isShorteningTrackRange) {
                anchorTrackIndex = clickedTrackIndex;
                startIndex = clickedTrackIndex;
                endIndex = maxSelectedIndex;
            } else {
                int anchorTrack = clickedTrackIndex;
                if (hasTrackRange) {
                    const int distanceToMin = std::abs(clickedTrackIndex - minSelectedIndex);
                    const int distanceToMax = std::abs(clickedTrackIndex - maxSelectedIndex);
                    anchorTrack = distanceToMin >= distanceToMax ? minSelectedIndex : maxSelectedIndex;
                } else if (lastAnchorTrackIndex >= 0) {
                    anchorTrack = lastAnchorTrackIndex;
                }
                anchorTrackIndex = anchorTrack;
                startIndex = std::min(anchorTrack, clickedTrackIndex);
                endIndex = std::max(anchorTrack, clickedTrackIndex);
            }

            selectionAnchorSample = anchorSample;
            selectionHoverSample = clickSample;
            hoverTrackIndex = -1;
            isSelecting = false;
            forceSelectionCommit = true;
            applySelectionRange(startIndex, endIndex);
            updateSelectionSamples();
            notifyListeners();
            return;
        }
    }
    anchorTrackIndex = clickedTrackIndex;
    selectionAnchorSample = clickSample;
    selectionHoverSample = selectionAnchorSample;
    if (!selectionAnchorSample.has_value()) {
        isSelecting = false;
        lastAnchorSample.reset();
        lastAnchorTrackIndex = -1;
        getCursorController().onSelectionCleared();
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
    if (relativeTo != nullptr) {
        selectionComponentWidth = relativeTo->getWidth();
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
    if (relativeTo != nullptr) {
        selectionComponentWidth = relativeTo->getWidth();
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
    if (relativeTo != nullptr) {
        selectionComponentWidth = relativeTo->getWidth();
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
    const auto commitAnchorSample = selectionAnchorSample;
    const int commitAnchorTrackIndex = anchorTrackIndex;
    if (selectionAnchorSample.has_value() && selectionHoverSample.has_value()) {
        const auto anchorSample = selectionAnchorSample.value();
        const auto hoverSample = selectionHoverSample.value();
        bool shouldCollapse = false;
        if (selectionComponentWidth > 0) {
            ViewRangeMapper mapper(edit, static_cast<float>(selectionComponentWidth));
            if (mapper.isValid()) {
                const float startX = mapper.sampleToX(anchorSample);
                const float endX = mapper.sampleToX(hoverSample);
                const float widthPx = std::abs(endX - startX);
                const float thresholdPx = static_cast<float>(selectionComponentWidth) * 0.002f;
                shouldCollapse = widthPx < thresholdPx;
            }
        }
        if (forceSelectionCommit) {
            shouldCollapse = false;
        }
        if (shouldCollapse) {
            getCursorController().setCursorSample(anchorSample);
            getCursorController().onSelectionCleared();
        } else {
            const auto rangeStart = std::min(anchorSample, hoverSample);
            getCursorController().setCursorSample(rangeStart);
        }
    } else {
        getCursorController().onSelectionCleared();
    }
    isSelecting = false;
    anchorTrackIndex = -1;
    hoverTrackIndex = -1;
    forceSelectionCommit = false;
    if (commitAnchorSample.has_value() && commitAnchorTrackIndex >= 0) {
        lastAnchorSample = commitAnchorSample;
        lastAnchorTrackIndex = commitAnchorTrackIndex;
    } else {
        lastAnchorSample.reset();
        lastAnchorTrackIndex = -1;
    }
    notifyListeners();
}

void SelectionManager::addTrackToSelection(const String& trackId) {
    if (selectedIds.find(trackId) != selectedIds.end()) {
        return;
    }
    std::vector<String> nextSelection;
    nextSelection.reserve(selectedIds.size() + 1);
    for (const auto& id : selectedIds) {
        nextSelection.push_back(id);
    }
    nextSelection.push_back(trackId);
    setSelection(nextSelection);
}

void SelectionManager::toggleTrackSelection(const String& trackId) {
    std::vector<String> nextSelection;
    nextSelection.reserve(selectedIds.size() + 1);
    bool removed = false;
    for (const auto& id : selectedIds) {
        if (id == trackId) {
            removed = true;
            continue;
        }
        nextSelection.push_back(id);
    }
    if (!removed) {
        nextSelection.push_back(trackId);
    }
    setSelection(nextSelection);
}

void SelectionManager::extendSelectionToTrack(const String& trackId) {
    int clickedIndex = -1;
    int minSelectedIndex = -1;
    int maxSelectedIndex = -1;
    int index = 0;
    for (const auto& track : edit.getTracks()) {
        if (!track) {
            ++index;
            continue;
        }
        const auto id = track->getId();
        if (id == trackId) {
            clickedIndex = index;
        }
        if (selectedIds.find(id) != selectedIds.end()) {
            if (minSelectedIndex < 0) {
                minSelectedIndex = index;
            }
            maxSelectedIndex = index;
        }
        ++index;
    }
    if (clickedIndex < 0) {
        return;
    }
    if (minSelectedIndex < 0) {
        setSelection({ trackId });
        return;
    }
    const bool clickedInsideRange = clickedIndex >= minSelectedIndex && clickedIndex <= maxSelectedIndex;
    if (clickedInsideRange) {
        const int centerIndex = minSelectedIndex + ((maxSelectedIndex - minSelectedIndex) / 2);
        const int startIndex = clickedIndex <= centerIndex ? clickedIndex : minSelectedIndex;
        const int endIndex = clickedIndex <= centerIndex ? maxSelectedIndex : clickedIndex;
        applySelectionRange(startIndex, endIndex);
        return;
    }

    const int startIndex = std::min(minSelectedIndex, clickedIndex);
    const int endIndex = std::max(maxSelectedIndex, clickedIndex);
    applySelectionRange(startIndex, endIndex);
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
    applySelectionRange(startIndex, endIndex);
}

void SelectionManager::applySelectionRange(int startIndex, int endIndex) {
    if (startIndex < 0 || endIndex < 0) {
        return;
    }
    if (startIndex > endIndex) {
        std::swap(startIndex, endIndex);
    }
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
        getCursorController().onSelectionCleared();
        return;
    }
    applySelectionSamples(selectionAnchorSample.value(), selectionHoverSample.value());
}

void SelectionManager::applySelectionSamples(int64 anchorSample, int64 hoverSample) {
    getCursorController().onSelectionRangeChanged(anchorSample, hoverSample);
    getCursorController().setCursorSample(std::min(anchorSample, hoverSample));
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
