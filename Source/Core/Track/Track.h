#pragma once

#include <JuceHeader.h>
#include <atomic>
#include <Utils/Format.h>
#include <Core/Automation/Automation.h>

#include "Core/Track/TrackState.h"
#include "Core/Plugin/Plugin.h"

class Send;
class EditState;
class FolderTrack;

/// Track category used for routing and UI.
enum class TrackType {
    Audio,
    Record,
    Aux,
    Folder,
    Unknown
};

/// Base track model with routing, plugins, and automation.
class Track : private juce::ValueTree::Listener {
public:
    /// Create a track with an optional name.
    /// @param name display name
    Track(String name = "");
    virtual ~Track();

    /// Add a send to this track.
    /// @param send send to add
    virtual void addSend(std::unique_ptr<Send> send);

    /// Access sends attached to this track.
    const std::vector<std::unique_ptr<Send>>& getSends();

    /// Bind track runtime state to the edit state.
    /// @param editState edit state that owns this track's ValueTree
    void bindState(EditState& editState);

    /// Runtime arm state from the edit state.
    TrackArmState getArmState() const;

    /// Runtime input monitoring state from the edit state.
    TrackInputMonitoringState getInputMonitoringState() const;

    /// Runtime solo state from the edit state.
    TrackSoloState getSoloState() const;

    /// Runtime solo-safe flag from the edit state.
    bool isSoloSafeEnabled() const;

    /// Runtime mute state from the edit state.
    TrackMuteState getMuteState() const;

    /// Add a plugin to this track.
    /// @param plugin plugin to add
    void addPlugin(std::shared_ptr<Plugin> plugin) {
        plugins.push_back(std::move(plugin));
    }

    /// Access plugins attached to this track.
    const std::vector<std::shared_ptr<Plugin>>& getPlugins() const {
        return plugins;
    }

    /// Route this track to the given output track.
    /// @param newOutputTrack output track to use
    void setOutput(std::weak_ptr<Track> newOutputTrack);

    /// Track id.
    String getId() const {
        return id;
    }

    /// Track display name.
    String getName() const {
        return name;
    }

    /// Output track (may be empty).
    std::weak_ptr<Track> getOutput() {
        return outputTrack;
    }

    /// Parent folder for this track (nullable).
    FolderTrack* getParentFolder() const {
        return parentFolder;
    }

    /// Assign the parent folder for this track.
    /// @param newParentFolder parent folder (nullable)
    void setParentFolder(FolderTrack* newParentFolder) {
        parentFolder = newParentFolder;
    }

    /// True if this is an audio track.
    [[nodiscard]] bool isAudioTrack() const {
        return isAudioTrack_;
    }

    /// Track type for routing.
    [[nodiscard]] TrackType getTrackType() const {
        return trackType;
    }

    /// Channel format for this track.
    ChannelsFormat getFormat() {
        return format;
    }

    float getHeight(){
        return height;
    }

    [[nodiscard]] juce::Colour getColour() const {
        return colour;
    }

protected:
    /// Update the cached arm state.
    /// @param state new arm state
    void setArmState(TrackArmState state);

    /// Update the cached input monitoring state.
    /// @param state new input monitoring state
    void setInputMonitoringState(TrackInputMonitoringState state);

    /// Update the cached solo state.
    /// @param state new solo state
    void setSoloState(TrackSoloState state);

    /// Update the cached solo-safe flag.
    /// @param soloSafe new solo-safe flag
    void setSoloSafeEnabled(bool soloSafe);

    /// Update the cached mute state.
    /// @param state new mute state
    void setMuteState(TrackMuteState state);

protected:
    String id;
    std::vector<std::shared_ptr<Plugin>> plugins;
    std::vector<std::unique_ptr<Send>> sends;
    std::weak_ptr<Track> outputTrack;
    std::weak_ptr<Track> inputTrack;
    std::vector<Automation> pluginsAutomation;
    Automation volumeAutomation;
    Automation panAutomation;
    std::vector<Automation> sendsAutomation;
    bool focused = false;
    int automationMode;
    bool solo;
    bool soloSafe;
    bool mute;
    FolderTrack* parentFolder = nullptr;
    ChannelsFormat format;
    ChannelsFormat outputFormat;
    bool isAudioTrack_ = false;
    TrackType trackType = TrackType::Unknown;
    float height;
    String name;
    juce::Colour colour;

private:
    struct RuntimeState {
        std::atomic<int> arm { static_cast<int>(TrackArmState::Inactive) };
        std::atomic<int> inputMonitoring { static_cast<int>(TrackInputMonitoringState::Inactive) };
        std::atomic<int> solo { static_cast<int>(TrackSoloState::Inactive) };
        std::atomic<bool> soloSafe { false };
        std::atomic<int> mute { static_cast<int>(TrackMuteState::Active) };
    };

    void detachState();
    void syncFromEditState();
    void valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier&) override;

    RuntimeState runtimeState;
    EditState* boundState = nullptr;
    juce::ValueTree trackStateNode;
};
