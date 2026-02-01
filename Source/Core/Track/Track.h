#pragma once

#include <JuceHeader.h>
#include <Utils/Format.h>
#include <Core/Automation/Automation.h>

#include "Core/Plugin/Plugin.h"

class Send;

/// Track category used for routing and UI.
enum class TrackType {
    Audio,
    Aux,
    Folder,
    Unknown
};

/// Base track model with routing, plugins, and automation.
class Track {
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
    String getId() {
        return id;
    }

    /// Track display name.
    String getName() {
        return name;
    }

    /// Output track (may be empty).
    std::weak_ptr<Track> getOutput() {
        return outputTrack;
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
    ChannelsFormat format;
    ChannelsFormat outputFormat;
    bool isAudioTrack_ = false;
    TrackType trackType = TrackType::Unknown;
    float height;
    String name;
    juce::Colour colour;
};
