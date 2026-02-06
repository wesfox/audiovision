#include "EditState.h"

#include <algorithm>
#include <cmath>

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
const juce::Identifier kTrackSoloSafeStateId("trackSoloSafeState");
const juce::Identifier kTrackMuteStateId("trackMuteState");
const juce::Identifier kTrackUserMuteStateId("trackUserMuteState");
const juce::Identifier kViewStartSampleFId("viewStartSampleF");
const juce::Identifier kSamplesPerPixelId("samplesPerPixel");
const juce::Identifier kViewWidthPixelsId("viewWidthPixels");
constexpr double kMinSamplesPerPixel = 1.0 / 16.0;
constexpr double kMaxSamplesPerPixel = 262144.0;
const juce::Identifier kFrameRateId("frameRate");
const juce::Identifier kTimelineHeightId("timelineHeight");
const juce::Identifier kHeaderHeightId("headerHeight");
const juce::Identifier kInsertionFollowsPlaybackId("insertionFollowsPlayback");
const juce::Identifier kIsLoopingId("isLooping");
const juce::Identifier kSoloModeId("soloMode");
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

double getDoubleProperty(const juce::ValueTree& tree, const juce::Identifier& propertyId, double fallback) {
    if (!tree.hasProperty(propertyId)) {
        return fallback;
    }
    return static_cast<double>(tree.getProperty(propertyId));
}

double snapSamplesPerPixel(double samplesPerPixel) {
    const auto exponent = std::log2(samplesPerPixel);
    const auto snapped = std::pow(2.0, std::round(exponent));
    return std::clamp(snapped, kMinSamplesPerPixel, kMaxSamplesPerPixel);
}
}

EditState::EditState() {
    root = juce::ValueTree(kEditStateType);
    globals = juce::ValueTree(kGlobalsType);
    viewState = juce::ValueTree(kViewType);
    trackState = juce::ValueTree(kTracksType);

    constexpr double kDefaultViewWidthPixels = 1000.0;
    constexpr double kDefaultViewLengthSamples = 48000.0 * 30.0;
    const auto defaultSamplesPerPixel = kDefaultViewLengthSamples / kDefaultViewWidthPixels;
    viewState.setProperty(kViewStartSampleFId, 0.0, nullptr);
    viewState.setProperty(kSamplesPerPixelId, defaultSamplesPerPixel, nullptr);
    viewState.setProperty(kViewWidthPixelsId, static_cast<float>(kDefaultViewWidthPixels), nullptr);
    globals.setProperty(kFrameRateId, 24.0f, nullptr);
    globals.setProperty(kTimelineHeightId, 20, nullptr);
    globals.setProperty(kHeaderHeightId, 90, nullptr);
    globals.setProperty(kWaveformScaleId, 1.0f, nullptr);
    globals.setProperty(kInsertionFollowsPlaybackId, true, nullptr);
    globals.setProperty(kIsLoopingId, false, nullptr);
    globals.setProperty(kSoloModeId, static_cast<int>(EditState::SoloMode::Latch), nullptr);
    globals.setProperty(kHasSelectionRangeId, false, nullptr);
    globals.setProperty(kSelectionStartSampleId, static_cast<int64>(0), nullptr);
    globals.setProperty(kSelectionEndSampleId, static_cast<int64>(0), nullptr);
    globals.setProperty(kCursorSampleId, static_cast<int64>(0), nullptr);

    root.addChild(globals, -1, nullptr);
    root.addChild(viewState, -1, nullptr);
    root.addChild(trackState, -1, nullptr);
}

double EditState::getViewStartSampleF() const {
    return getDoubleProperty(viewState, kViewStartSampleFId, 0.0);
}

double EditState::getSamplesPerPixel() const {
    return getDoubleProperty(viewState, kSamplesPerPixelId, 1.0);
}

float EditState::getViewWidthPixels() const {
    return getFloatProperty(viewState, kViewWidthPixelsId, 1.0f);
}

int64 EditState::getViewStartSample() const {
    return static_cast<int64>(std::llround(getViewStartSampleF()));
}

int64 EditState::getViewEndSample() const {
    return getViewStartSample() + getViewLengthSamples();
}

int64 EditState::getViewLengthSamples() const {
    const auto width = static_cast<double>(getViewWidthPixels());
    const auto samplesPerPixel = getSamplesPerPixel();
    if (width <= 0.0 || samplesPerPixel <= 0.0) {
        // View width and scale must be positive to compute length.
        jassert(false);
        return 1;
    }
    return std::max<int64>(1, static_cast<int64>(std::llround(samplesPerPixel * width)));
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

EditState::SoloMode EditState::getSoloMode() const {
    const auto raw = static_cast<int>(getInt64Property(globals,
                                                       kSoloModeId,
                                                       static_cast<int>(SoloMode::Latch)));
    return static_cast<SoloMode>(raw);
}

bool EditState::getInsertionFollowsPlayback() const {
    return getBoolProperty(globals, kInsertionFollowsPlaybackId, true);
}

bool EditState::getIsLooping() const {
    return getBoolProperty(globals, kIsLoopingId, false);
}

void EditState::setViewStartSampleF(double startSample, juce::UndoManager* undo) {
    if (startSample < 0.0) {
        // View start must be non-negative.
        jassert(false);
        startSample = 0.0;
    }
    viewState.setProperty(kViewStartSampleFId, startSample, undo);
}

void EditState::setSamplesPerPixel(double samplesPerPixel, juce::UndoManager* undo) {
    if (samplesPerPixel <= 0.0) {
        // Samples-per-pixel must be positive.
        jassert(false);
        return;
    }
    viewState.setProperty(kSamplesPerPixelId, snapSamplesPerPixel(samplesPerPixel), undo);
}

void EditState::setViewWidthPixels(float widthPixels, juce::UndoManager* undo) {
    if (widthPixels <= 0.0f) {
        // View width must be positive.
        jassert(false);
        return;
    }
    viewState.setProperty(kViewWidthPixelsId, widthPixels, undo);
}

void EditState::setViewRange(int64 startSample, int64 endSample, juce::UndoManager* undo) {
    const auto width = static_cast<double>(getViewWidthPixels());
    if (width <= 0.0) {
        // View width must be positive to derive samples-per-pixel.
        jassert(false);
        return;
    }
    const auto clampedEnd = std::max<int64>(startSample + 1, endSample);
    const auto lengthSamples = static_cast<double>(clampedEnd - startSample);
    setViewStartSampleF(static_cast<double>(startSample), undo);
    setSamplesPerPixel(lengthSamples / width, undo);
}

void EditState::zoom(float ratio, int64 centerSample, juce::UndoManager* undo) {
    const auto width = static_cast<double>(getViewWidthPixels());
    const auto currentSamplesPerPixel = getSamplesPerPixel();
    if (width <= 0.0 || currentSamplesPerPixel <= 0.0) {
        // View width and scale must be positive to compute zoom.
        jassert(false);
        return;
    }
    if (ratio == 0.0f) {
        return;
    }

    const auto zoomOut = ratio > 0.0f;
    auto nextSamplesPerPixel = zoomOut
        ? (currentSamplesPerPixel * 2.0)
        : (currentSamplesPerPixel * 0.5);
    nextSamplesPerPixel = std::clamp(nextSamplesPerPixel, kMinSamplesPerPixel, kMaxSamplesPerPixel);
    if (nextSamplesPerPixel == currentSamplesPerPixel) {
        return;
    }
    if (nextSamplesPerPixel <= 0.0) {
        // Samples-per-pixel must stay positive.
        jassert(false);
        return;
    }

    const auto viewStartSampleF = getViewStartSampleF();
    const auto viewLength = currentSamplesPerPixel * width;
    const auto nextLength = nextSamplesPerPixel * width;
    auto newStart = viewStartSampleF;
    const auto viewEndSampleF = viewStartSampleF + viewLength;
    const auto centerSampleF = static_cast<double>(centerSample);
    if (centerSampleF >= viewStartSampleF && centerSampleF <= viewEndSampleF) {
        const auto anchorRatio = (centerSampleF - viewStartSampleF) / viewLength;
        newStart = centerSampleF - (anchorRatio * nextLength);
    } else {
        const auto viewCenter = viewStartSampleF + (viewLength * 0.5);
        newStart = viewCenter - (nextLength * 0.5);
    }

    if (newStart < 0.0) {
        newStart = 0.0;
    }

    setViewStartSampleF(newStart, undo);
    setSamplesPerPixel(nextSamplesPerPixel, undo);
}

int64 EditState::mapPixelToSample(float pixelX) const {
    const auto width = static_cast<double>(getViewWidthPixels());
    const auto samplesPerPixel = getSamplesPerPixel();
    if (width <= 0.0 || samplesPerPixel <= 0.0) {
        // View width and scale must be positive to map pixels.
        jassert(false);
        return getViewStartSample();
    }
    const auto clampedX = static_cast<double>(juce::jlimit(0.0f, static_cast<float>(width), pixelX));
    const auto sample = getViewStartSampleF() + (clampedX * samplesPerPixel);
    return static_cast<int64>(std::llround(sample));
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

void EditState::setSoloMode(SoloMode mode, juce::UndoManager* undo) {
    globals.setProperty(kSoloModeId, static_cast<int>(mode), undo);
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

bool EditState::getTrackSoloSafeState(const String& trackId) const {
    const auto trackNode = getTrackState(trackId);
    if (!trackNode.isValid()) {
        return false;
    }
    return static_cast<bool>(trackNode.getProperty(kTrackSoloSafeStateId, false));
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

void EditState::setTrackSoloSafeState(const String& trackId, bool soloSafe, juce::UndoManager* undo) {
    auto trackNode = getOrCreateTrackState(trackId);
    trackNode.setProperty(kTrackSoloSafeStateId, soloSafe, undo);
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
    trackNode.setProperty(kTrackSoloSafeStateId, false, nullptr);
    trackNode.setProperty(kTrackMuteStateId, static_cast<int>(TrackMuteState::Active), nullptr);
    trackNode.setProperty(kTrackUserMuteStateId, static_cast<int>(TrackMuteState::Active), nullptr);
    trackState.addChild(trackNode, -1, nullptr);
    return trackNode;
}
