#pragma once

#include <JuceHeader.h>
#include <Utils/Format.h>
#include <Core/Automation/Automation.h>

class Send;

class Track {
public:
    Track(String name = "");
    virtual ~Track();

    // Methods
    virtual void addSend(std::unique_ptr<Send> send);

    const std::vector<std::unique_ptr<Send>>& getSends();

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

    ChannelsFormat getFormat() {
        return format;
    }

protected:
    String id;
    std::vector<void*> plugins;
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
    float height;
    String name;
    juce::Colour color;
};
