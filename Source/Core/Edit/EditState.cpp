#include "EditState.h"

namespace {
const juce::Identifier kEditStateType("EDIT_STATE");
const juce::Identifier kGlobalsType("GLOBALS");
const juce::Identifier kViewType("VIEW");
const juce::Identifier kTracksType("TRACKS");
const juce::Identifier kTrackType("TRACK");
const juce::Identifier kTrackIdPropertyId("trackId");
const juce::Identifier kTrackArmStateId("trackArmState");
const juce::Identifier kTrackInputMonitoringStateId("trackInputMonitoringState");
const juce::Identifier kTrackSoloStateId("trackSoloState");
const juce::Identifier kTrackMuteStateId("trackMuteState");
const juce::Identifier kTrackUserMuteStateId("trackUserMuteState");
const juce::Identifier kViewStartSampleId("viewStartSample");
const juce::Identifier kViewEndSampleId("viewEndSample");
const juce::Identifier kFrameRateId("frameRate");
const juce::Identifier kTimelineHeightId("timelineHeight");
const juce::Identifier kHeaderHeightId("headerHeight");
const juce::Identifier kInsertionFollowsPlaybackId("insertionFollowsPlayback");
const juce::Identifier kIsLoopingId("isLooping");
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
    trackState = juce::ValueTree(kTracksType);

    viewState.setProperty(kViewStartSampleId, static_cast<int64>(0), nullptr);
    viewState.setProperty(kViewEndSampleId, static_cast<int64>(48000 * 30), nullptr);
    globals.setProperty(kFrameRateId, 24.0f, nullptr);
    globals.setProperty(kTimelineHeightId, 20, nullptr);
    globals.setProperty(kHeaderHeightId, 90, nullptr);
    globals.setProperty(kWaveformScaleId, 1.0f, nullptr);
    globals.setProperty(kInsertionFollowsPlaybackId, true, nullptr);
    globals.setProperty(kIsLoopingId, false, nullptr);
    globals.setProperty(kHasSelectionRangeId, false, nullptr);
    globals.setProperty(kSelectionStartSampleId, static_cast<int64>(0), nullptr);
    globals.setProperty(kSelectionEndSampleId, static_cast<int64>(0), nullptr);
    globals.setProperty(kCursorSampleId, static_cast<int64>(0), nullptr);

    root.addChild(globals, -1, nullptr);
    root.addChild(viewState, -1, nullptr);
    root.addChild(trackState, -1, nullptr);
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

bool EditState::getIsLooping() const {
    return getBoolProperty(globals, kIsLoopingId, false);
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

void EditState::setIsLooping(bool isLooping, juce::UndoManager* undo) {
    globals.setProperty(kIsLoopingId, isLooping, undo);
}

void EditState::ensureTrackState(const String& trackId) {
    getOrCreateTrackState(trackId);
}

TrackArmState EditState::getTrackArmState(const String& trackId) const {
    const auto trackNode = getTrackState(trackId);
    if (!trackNode.isValid()) {
        return TrackArmState::Inactive;
    }
    const auto raw = static_cast<int>(trackNode.getProperty(kTrackArmStateId,
                                                            static_cast<int>(TrackArmState::Inactive)));
    return static_cast<TrackArmState>(raw);
}

TrackInputMonitoringState EditState::getTrackInputMonitoringState(const String& trackId) const {
    const auto trackNode = getTrackState(trackId);
    if (!trackNode.isValid()) {
        return TrackInputMonitoringState::Inactive;
    }
    const auto raw = static_cast<int>(trackNode.getProperty(kTrackInputMonitoringStateId,
                                                            static_cast<int>(TrackInputMonitoringState::Inactive)));
    return static_cast<TrackInputMonitoringState>(raw);
}

TrackSoloState EditState::getTrackSoloState(const String& trackId) const {
    const auto trackNode = getTrackState(trackId);
    if (!trackNode.isValid()) {
        return TrackSoloState::Inactive;
    }
    const auto raw = static_cast<int>(trackNode.getProperty(kTrackSoloStateId,
                                                            static_cast<int>(TrackSoloState::Inactive)));
    return static_cast<TrackSoloState>(raw);
}

TrackMuteState EditState::getTrackMuteState(const String& trackId) const {
    const auto trackNode = getTrackState(trackId);
    if (!trackNode.isValid()) {
        return TrackMuteState::Active;
    }
    const auto raw = static_cast<int>(trackNode.getProperty(kTrackMuteStateId,
                                                            static_cast<int>(TrackMuteState::Active)));
    return static_cast<TrackMuteState>(raw);
}

TrackMuteState EditState::getTrackUserMuteState(const String& trackId) const {
    const auto trackNode = getTrackState(trackId);
    if (!trackNode.isValid()) {
        return TrackMuteState::Active;
    }
    const auto raw = static_cast<int>(trackNode.getProperty(kTrackUserMuteStateId,
                                                            static_cast<int>(TrackMuteState::Active)));
    return static_cast<TrackMuteState>(raw);
}

void EditState::setTrackArmState(const String& trackId, TrackArmState state, juce::UndoManager* undo) {
    auto trackNode = getOrCreateTrackState(trackId);
    trackNode.setProperty(kTrackArmStateId, static_cast<int>(state), undo);
}

void EditState::setTrackInputMonitoringState(const String& trackId, TrackInputMonitoringState state, juce::UndoManager* undo) {
    auto trackNode = getOrCreateTrackState(trackId);
    trackNode.setProperty(kTrackInputMonitoringStateId, static_cast<int>(state), undo);
}

void EditState::setTrackSoloState(const String& trackId, TrackSoloState state, juce::UndoManager* undo) {
    auto trackNode = getOrCreateTrackState(trackId);
    trackNode.setProperty(kTrackSoloStateId, static_cast<int>(state), undo);
}

void EditState::setTrackMuteState(const String& trackId, TrackMuteState state, juce::UndoManager* undo) {
    auto trackNode = getOrCreateTrackState(trackId);
    trackNode.setProperty(kTrackMuteStateId, static_cast<int>(state), undo);
}

void EditState::setTrackUserMuteState(const String& trackId, TrackMuteState state, juce::UndoManager* undo) {
    auto trackNode = getOrCreateTrackState(trackId);
    trackNode.setProperty(kTrackUserMuteStateId, static_cast<int>(state), undo);
}

juce::ValueTree EditState::getTrackState(const String& trackId) const {
    return trackState.getChildWithProperty(kTrackIdPropertyId, trackId);
}

juce::ValueTree EditState::getOrCreateTrackState(const String& trackId) {
    auto trackNode = getTrackState(trackId);
    if (trackNode.isValid()) {
        return trackNode;
    }

    trackNode = juce::ValueTree(kTrackType);
    trackNode.setProperty(kTrackIdPropertyId, trackId, nullptr);
    trackNode.setProperty(kTrackArmStateId, static_cast<int>(TrackArmState::Inactive), nullptr);
    trackNode.setProperty(kTrackInputMonitoringStateId, static_cast<int>(TrackInputMonitoringState::Inactive), nullptr);
    trackNode.setProperty(kTrackSoloStateId, static_cast<int>(TrackSoloState::Inactive), nullptr);
    trackNode.setProperty(kTrackMuteStateId, static_cast<int>(TrackMuteState::Active), nullptr);
    trackNode.setProperty(kTrackUserMuteStateId, static_cast<int>(TrackMuteState::Active), nullptr);
    trackState.addChild(trackNode, -1, nullptr);
    return trackNode;
}
