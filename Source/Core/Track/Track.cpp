#include "Track.h"
#include "Core/Edit/EditState.h"
#include <Utils/Uuid.h>

#include <utility>
#include "Send.h"

Track::Track(String name)
    : id(uuid::generate_uuid_v4()),
      outputTrack({}),
      inputTrack({}),
      automationMode(0),
      solo(false),
      soloSafe(false),
      mute(false),
      format(ChannelsFormat::Stereo),
      outputFormat(ChannelsFormat::Stereo),
      height(100.0f),
      name(std::move(name)),
      colour(juce::Colour(0xFF63A129))
{
}

Track::~Track()
{
    detachState();
}

void Track::bindState(EditState& editState)
{
    if (id.isEmpty()) {
        // Track ids must be valid before binding state.
        jassert(false);
        return;
    }

    detachState();
    boundState = &editState;
    trackStateNode = editState.getTrackState(id);
    if (!trackStateNode.isValid()) {
        editState.ensureTrackState(id);
        trackStateNode = editState.getTrackState(id);
    }
    if (!trackStateNode.isValid()) {
        // Track state nodes must exist for bound tracks.
        jassert(false);
        return;
    }

    trackStateNode.addListener(this);
    syncFromEditState();
}

TrackArmState Track::getArmState() const
{
    return static_cast<TrackArmState>(runtimeState.arm.load(std::memory_order_relaxed));
}

TrackInputMonitoringState Track::getInputMonitoringState() const
{
    return static_cast<TrackInputMonitoringState>(runtimeState.inputMonitoring.load(std::memory_order_relaxed));
}

TrackSoloState Track::getSoloState() const
{
    return static_cast<TrackSoloState>(runtimeState.solo.load(std::memory_order_relaxed));
}

TrackMuteState Track::getMuteState() const
{
    return static_cast<TrackMuteState>(runtimeState.mute.load(std::memory_order_relaxed));
}

void Track::setArmState(TrackArmState state)
{
    runtimeState.arm.store(static_cast<int>(state), std::memory_order_relaxed);
}

void Track::setInputMonitoringState(TrackInputMonitoringState state)
{
    runtimeState.inputMonitoring.store(static_cast<int>(state), std::memory_order_relaxed);
}

void Track::setSoloState(TrackSoloState state)
{
    runtimeState.solo.store(static_cast<int>(state), std::memory_order_relaxed);
}

void Track::setMuteState(TrackMuteState state)
{
    runtimeState.mute.store(static_cast<int>(state), std::memory_order_relaxed);
}

void Track::addSend(std::unique_ptr<Send> send)
{
    sends.push_back(std::move(send));
}

const std::vector<std::unique_ptr<Send>>& Track::getSends()
{
    return sends;
}

void Track::setOutput(std::weak_ptr<Track> newOutputTrack) {
    outputTrack = newOutputTrack;
}

void Track::detachState()
{
    if (trackStateNode.isValid()) {
        trackStateNode.removeListener(this);
    }
    trackStateNode = {};
    boundState = nullptr;
}

void Track::syncFromEditState()
{
    if (boundState == nullptr) {
        return;
    }

    setArmState(boundState->getTrackArmState(id));
    setInputMonitoringState(boundState->getTrackInputMonitoringState(id));
    setSoloState(boundState->getTrackSoloState(id));
    setMuteState(boundState->getTrackMuteState(id));
}

void Track::valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier&)
{
    syncFromEditState();
}
