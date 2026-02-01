#include "TrackContentPanel.h"

#include <algorithm>
#include <cmath>

#include "TrackContent.h"

// ------------------------ MainComponent Implementation ------------------------

TrackContentPanel::TrackContentPanel(Edit& edit, SelectionManager& selectionManager)
    : edit(edit),
      selectionManager(selectionManager) {
    const auto rulerHeight = edit.getState().getTimelineHeight();
    timelineRuler = std::make_unique<TimelineRuler>(edit, rulerHeight);
    addAndMakeVisible(timelineRuler.get());
    playheadController = std::make_unique<PlayheadFollowController>(edit);
    for (const auto& track : edit.getTracks()) {
        if (!track) {
            continue;
        }
        auto trackContent = std::make_shared<TrackContent>(edit, selectionManager, track);
        addAndMakeVisible(*trackContent);
        trackContentComponents.emplace(track->getId(), std::move(trackContent));
    }
    cursorTimeline = std::make_unique<CursorTimeline>(edit, rulerHeight);
    cursorTimeline->setCallbacks({
        [this](int64 previousSample, int64 newSample) { playheadController->onPointerDown(previousSample, newSample); },
        [this](int64 sample) { playheadController->onPointerDrag(sample); },
        [this](int64 previousSample, int64 newSample, bool wasDrag) {
            playheadController->onPointerUp(previousSample, newSample, wasDrag);
        }
    });
    addAndMakeVisible(cursorTimeline.get());
    edit.getState().getRoot().addListener(this);
    startTimerHz(30);
    addMouseListener(this, true);
}

TrackContentPanel::~TrackContentPanel() {
    edit.getState().getRoot().removeListener(this);
}

void TrackContentPanel::resized() {
    updateLayout();
}

void TrackContentPanel::updateLayout() {
    const auto rulerHeight = edit.getState().getTimelineHeight();
    auto fullBounds = getLocalBounds();
    if (cursorTimeline != nullptr) {
        cursorTimeline->setRulerHeight(rulerHeight);
        cursorTimeline->setBounds(fullBounds);
    }
    auto bounds = fullBounds;
    if (timelineRuler != nullptr) {
        timelineRuler->setRulerHeight(rulerHeight);
        timelineRuler->setBounds(bounds.removeFromTop(rulerHeight));
    }
    for (const auto& track : edit.getTracks()) {
        if (!track) {
            continue;
        }
        auto it = trackContentComponents.find(track->getId());
        if (it == trackContentComponents.end()) {
            continue;
        }
        it->second->setBounds(bounds.removeFromTop(std::max(1, static_cast<int>(track->getHeight()))));
        it->second->updateLayout();
    }
}

void TrackContentPanel::paint(juce::Graphics& g) {
    const auto rulerHeight = edit.getState().getTimelineHeight();
    auto bounds = getLocalBounds();
    juce::Path clipPath;
    clipPath.addRoundedRectangle(bounds.toFloat(), 6.0f);
    g.saveState();
    g.reduceClipRegion(clipPath);
    bounds.removeFromTop(rulerHeight);
    g.setColour(juce::Colour::fromRGB(245, 245, 245));
    g.fillRect(bounds);

    const auto viewStart = edit.getViewStartSample();
    const auto viewEnd = edit.getViewEndSample();
    const auto viewLength = viewEnd - viewStart;
    if (viewLength <= 0) {
        return;
    }

    const float width = static_cast<float>(bounds.getWidth());
    const float height = static_cast<float>(bounds.getHeight());

    int sceneIndex = 0;
    for (const auto& scene : edit.getScenes()) {
        if (!scene) {
            continue;
        }

        const auto sceneStart = scene->sceneStartSample;
        const auto sceneEnd = scene->sceneEndSample;
        if (sceneEnd <= viewStart || sceneStart >= viewEnd) {
            continue;
        }

        const auto clippedStart = std::max(sceneStart, viewStart);
        const auto clippedEnd = std::min(sceneEnd, viewEnd);
        const float startX = (static_cast<float>(clippedStart - viewStart) / static_cast<float>(viewLength)) * width;
        const float endX = (static_cast<float>(clippedEnd - viewStart) / static_cast<float>(viewLength)) * width;
        const float sceneWidth = std::max(1.0f, endX - startX);

        auto color = (sceneIndex % 2 == 0)
            ? juce::Colour(0xFFF6F4FF)
            : juce::Colour(0xFFE7E1FB);
        g.setColour(color);
        g.fillRect(startX, 0.0f, sceneWidth, height);
        sceneIndex++;
    }


    const double frameRate = static_cast<double>(edit.getFrameRate());
    if (frameRate > 0.0) {
        const double sampleRate = 48000.0;
        const double samplesPerFrame = sampleRate / frameRate;

        const auto firstFrameIndex = static_cast<int64>(std::floor(static_cast<double>(viewStart) / samplesPerFrame));
        const auto lastFrameIndex = static_cast<int64>(std::ceil(static_cast<double>(viewEnd) / samplesPerFrame));

        auto spaceBetweenFrames = width/static_cast<double>(lastFrameIndex - firstFrameIndex);

        double startShowingFrameSpacing = 4.0;
        double fullOpacityFameSpacing = 6.0;
        double frameLineMaxOpacity = 50;
        if (spaceBetweenFrames > startShowingFrameSpacing){
            int frameLineOpacity = frameLineMaxOpacity - (fullOpacityFameSpacing-(std::min(spaceBetweenFrames, fullOpacityFameSpacing)))*(frameLineMaxOpacity/(fullOpacityFameSpacing-startShowingFrameSpacing));
            const auto lineColour = juce::Colour::fromRGBA(20, 20, 20, frameLineOpacity);
            g.setColour(lineColour);
            for (int64 frameIndex = firstFrameIndex; frameIndex <= lastFrameIndex; ++frameIndex) {
                const double frameSample = static_cast<double>(frameIndex) * samplesPerFrame;
                float x = static_cast<float>((frameSample - static_cast<double>(viewStart)) / static_cast<double>(viewLength)) * width;
                x = std::floor(x) + 0.5f;
                if (x < 0.0f || x > width) {
                    continue;
                }
                g.drawLine(x, 0.0f, x, height, 1.0f);
            }
        }
    }
    g.restoreState();
}

void TrackContentPanel::valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier&) {
    triggerAsyncUpdate();
}

void TrackContentPanel::handleAsyncUpdate() {
    updateLayout();
    repaint();
}

void TrackContentPanel::timerCallback() {
    const auto transport = edit.getTransport();
    // TODO : add transport.isPlaying(), it is okay, but that may create some perf issues
    if (!transport) {
        return;
    }

    if (cursorTimeline != nullptr) {
        cursorTimeline->repaint();
    }

    const auto playheadSample = transport->getCursorPosition();
    if (playheadController != nullptr) {
        playheadController->onPlaybackTick(playheadSample);
    }

    for (const auto& [trackId, trackContent] : trackContentComponents) {
        if (!trackContent) {
            continue;
        }
        if (selectionManager.isSelected(trackId)) {
            trackContent->repaint();
        }
    }
}

void TrackContentPanel::mouseDown(const juce::MouseEvent& event) {
    selectionManager.mouseDown(event, this);
    const auto transport = edit.getTransport();
    if (transport != nullptr) {
        if (transport->isPlaying()) {
            return;
        }
        const auto viewStart = edit.getViewStartSample();
        const auto viewEnd = edit.getViewEndSample();
        const auto viewLength = viewEnd - viewStart;
        if (viewLength <= 0) {
            // View length must be positive to map cursor position.
            jassert(false);
        } else {
            const auto relative = event.getEventRelativeTo(this);
            const float width = static_cast<float>(getWidth());
            const float x = juce::jlimit(0.0f, width, relative.position.x);
            const auto cursorSample = viewStart
                + static_cast<int64>(std::llround((x / width) * static_cast<float>(viewLength)));
            transport->setCursorPosition(cursorSample);
        }
    }
}

void TrackContentPanel::mouseDrag(const juce::MouseEvent& event) {
    selectionManager.mouseDrag(event, this);
}

void TrackContentPanel::mouseMove(const juce::MouseEvent& event) {
    selectionManager.mouseMove(event, this);
}

void TrackContentPanel::mouseEnter(const juce::MouseEvent& event) {
    selectionManager.mouseEnter(event, this);
}

void TrackContentPanel::mouseUp(const juce::MouseEvent&) {
    selectionManager.mouseUp();
}
