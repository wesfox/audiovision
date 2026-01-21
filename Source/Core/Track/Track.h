#pragma once

#include <JuceHeader.h>
#include <Utils/Format.h>
#include <Core/Automation/Automation.h>

#include "Core/Plugin/Plugin.h"

class Send;

enum class TrackType {
    Audio,
    Aux,
    Folder,
    Unknown
};

class Track {
public:
    Track(String name = "");
    virtual ~Track();

    // Methods
    virtual void addSend(std::unique_ptr<Send> send);

    const std::vector<std::unique_ptr<Send>>& getSends();

    void addPlugin(std::shared_ptr<Plugin> plugin) {
        plugins.push_back(std::move(plugin));
    }

    const std::vector<std::shared_ptr<Plugin>>& getPlugins() const {
        return plugins;
    }

    /// Make this track output to the defined track
    void setOutput(std::weak_ptr<Track> newOutputTrack);

    String getId() {
        return id;
    }

    String getName() {
        return name;
    }


    std::weak_ptr<Track> getOutput() {
        return outputTrack;
    }

    [[nodiscard]] bool isAudioTrack() const {
        return isAudioTrack_;
    }

    [[nodiscard]] TrackType getTrackType() const {
        return trackType;
    }

    ChannelsFormat getFormat() {
        return format;
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
    juce::Colour color;
};
