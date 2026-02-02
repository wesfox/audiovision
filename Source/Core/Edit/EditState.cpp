#include "EditState.h"

namespace {
const juce::Identifier kEditStateType("EDIT_STATE");
const juce::Identifier kGlobalsType("GLOBALS");
const juce::Identifier kViewType("VIEW");
const juce::Identifier kViewStartSampleId("viewStartSample");
const juce::Identifier kViewEndSampleId("viewEndSample");
const juce::Identifier kFrameRateId("frameRate");
const juce::Identifier kTimelineHeightId("timelineHeight");
const juce::Identifier kHeaderHeightId("headerHeight");
const juce::Identifier kInsertionFollowsPlaybackId("insertionFollowsPlayback");
const juce::Identifier kHasSelectionRangeId("hasSelectionRange");
const juce::Identifier kSelectionStartSampleId("selectionStartSample");
const juce::Identifier kSelectionEndSampleId("selectionEndSample");
const juce::Identifier kCursorSampleId("cursorSample");
}

const juce::Identifier EditState::kWaveformScaleId("waveformScale");

namespace {

int64 getInt64Property(const juce::ValueTree& tree, const juce::Identifier& propertyId, int64 fallback) {
    if (!tree.hasProperty(propertyId)) {
        return fallback;
    }
    return static_cast<int64>(tree.getProperty(propertyId));
}

float getFloatProperty(const juce::ValueTree& tree, const juce::Identifier& propertyId, float fallback) {
    if (!tree.hasProperty(propertyId)) {
        return fallback;
    }
    return static_cast<float>(tree.getProperty(propertyId));
}

bool getBoolProperty(const juce::ValueTree& tree, const juce::Identifier& propertyId, bool fallback) {
    if (!tree.hasProperty(propertyId)) {
        return fallback;
    }
    return static_cast<bool>(tree.getProperty(propertyId));
}
}

EditState::EditState() {
    root = juce::ValueTree(kEditStateType);
    globals = juce::ValueTree(kGlobalsType);
    viewState = juce::ValueTree(kViewType);

    viewState.setProperty(kViewStartSampleId, static_cast<int64>(0), nullptr);
    viewState.setProperty(kViewEndSampleId, static_cast<int64>(48000 * 30), nullptr);
    globals.setProperty(kFrameRateId, 24.0f, nullptr);
    globals.setProperty(kTimelineHeightId, 20, nullptr);
    globals.setProperty(kHeaderHeightId, 90, nullptr);
    globals.setProperty(kWaveformScaleId, 1.0f, nullptr);
    globals.setProperty(kInsertionFollowsPlaybackId, true, nullptr);
    globals.setProperty(kHasSelectionRangeId, false, nullptr);
    globals.setProperty(kSelectionStartSampleId, static_cast<int64>(0), nullptr);
    globals.setProperty(kSelectionEndSampleId, static_cast<int64>(0), nullptr);
    globals.setProperty(kCursorSampleId, static_cast<int64>(0), nullptr);

    root.addChild(globals, -1, nullptr);
    root.addChild(viewState, -1, nullptr);
}

int64 EditState::getViewStartSample() const {
    return getInt64Property(viewState, kViewStartSampleId, 0);
}

int64 EditState::getViewEndSample() const {
    return getInt64Property(viewState, kViewEndSampleId, 48000 * 30);
}

float EditState::getFrameRate() const {
    return getFloatProperty(globals, kFrameRateId, 25.0f);
}

int EditState::getTimelineHeight() const {
    return static_cast<int>(getInt64Property(globals, kTimelineHeightId, 20));
}

int EditState::getHeaderHeight() const {
    return static_cast<int>(getInt64Property(globals, kHeaderHeightId, 90));
}

float EditState::getWaveformScale() const {
    return getFloatProperty(globals, kWaveformScaleId, 1.0f);
}

bool EditState::hasSelectionRange() const {
    return getBoolProperty(globals, kHasSelectionRangeId, false);
}

int64 EditState::getSelectionStartSample() const {
    return getInt64Property(globals, kSelectionStartSampleId, 0);
}

int64 EditState::getSelectionEndSample() const {
    return getInt64Property(globals, kSelectionEndSampleId, 0);
}

int64 EditState::getCursorSample() const {
    return getInt64Property(globals, kCursorSampleId, 0);
}

bool EditState::getInsertionFollowsPlayback() const {
    return getBoolProperty(globals, kInsertionFollowsPlaybackId, true);
}

void EditState::setViewRange(int64 startSample, int64 endSample, juce::UndoManager* undo) {
    viewState.setProperty(kViewStartSampleId, startSample, undo);
    viewState.setProperty(kViewEndSampleId, endSample, undo);
}

void EditState::zoom(float ratio, int64 centerSample, juce::UndoManager* undo) {
    const auto viewStartSample = getViewStartSample();
    const auto viewEndSample = getViewEndSample();
    const auto viewLength = viewEndSample - viewStartSample;
    if (viewLength <= 0) {
        // View length must be positive to compute zoom.
        jassert(false);
        return;
    }

    const double nextLength = std::max(1.0, static_cast<double>(viewLength) * (1.0 + 2.0 * ratio));
    const double halfLength = nextLength * 0.5;
    auto newStart = static_cast<int64>(std::llround(static_cast<double>(centerSample) - halfLength));
    auto newEnd = static_cast<int64>(std::llround(static_cast<double>(centerSample) + halfLength));
    if (newStart < 0) {
        newStart = 0;
        newEnd = static_cast<int64>(std::llround(nextLength));
    }
    setViewRange(newStart, newEnd, undo);
}

void EditState::setFrameRate(float frameRate, juce::UndoManager* undo) {
    globals.setProperty(kFrameRateId, frameRate, undo);
}

void EditState::setTimelineHeight(int height, juce::UndoManager* undo) {
    globals.setProperty(kTimelineHeightId, height, undo);
}

void EditState::setHeaderHeight(int height, juce::UndoManager* undo) {
    globals.setProperty(kHeaderHeightId, height, undo);
}

void EditState::setWaveformScale(float scale, juce::UndoManager* undo) {
    globals.setProperty(kWaveformScaleId, scale, undo);
}

void EditState::setSelectionRange(int64 startSample, int64 endSample, juce::UndoManager* undo) {
    globals.setProperty(kHasSelectionRangeId, true, undo);
    globals.setProperty(kSelectionStartSampleId, startSample, undo);
    globals.setProperty(kSelectionEndSampleId, endSample, undo);
}

void EditState::setCursorSample(int64 sample, juce::UndoManager* undo) {
    globals.setProperty(kCursorSampleId, sample, undo);
}

void EditState::clearSelectionRange(juce::UndoManager* undo) {
    globals.setProperty(kHasSelectionRangeId, false, undo);
}

void EditState::setInsertionFollowsPlayback(bool followsPlayback, juce::UndoManager* undo) {
    globals.setProperty(kInsertionFollowsPlaybackId, followsPlayback, undo);
}
