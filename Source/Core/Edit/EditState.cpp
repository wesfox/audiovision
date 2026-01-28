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
}

EditState::EditState() {
    root = juce::ValueTree(kEditStateType);
    globals = juce::ValueTree(kGlobalsType);
    viewState = juce::ValueTree(kViewType);

    viewState.setProperty(kViewStartSampleId, static_cast<int64>(0), nullptr);
    viewState.setProperty(kViewEndSampleId, static_cast<int64>(48000 * 30), nullptr);
    globals.setProperty(kFrameRateId, 25.0f, nullptr);
    globals.setProperty(kTimelineHeightId, 20, nullptr);
    globals.setProperty(kHeaderHeightId, 90, nullptr);

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

void EditState::setViewRange(int64 startSample, int64 endSample, juce::UndoManager* undo) {
    viewState.setProperty(kViewStartSampleId, startSample, undo);
    viewState.setProperty(kViewEndSampleId, endSample, undo);
}

void EditState::zoom(float ratio, juce::UndoManager* undo) {
    auto viewStartSample = getViewStartSample();
    auto viewEndSample = getViewEndSample();
    viewEndSample = viewEndSample + static_cast<float>(viewEndSample - viewStartSample) * ratio;
    viewStartSample = viewStartSample - static_cast<float>(viewEndSample - viewStartSample) * ratio;
    viewStartSample = std::max<int64>(0, viewStartSample);
    juce::Logger::writeToLog(String(ratio) + "-" + String(viewStartSample) + "-" + String(viewEndSample));
    setViewRange(viewStartSample, viewEndSample, undo);
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
