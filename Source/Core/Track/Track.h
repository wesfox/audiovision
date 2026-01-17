#pragma once

#include <JuceHeader.h>
#include <Utils/Format.h>
#include <Core/Automation/Automation.h>

class Track {
public:
    Track();
    virtual ~Track() = default;

    // Methods
    virtual void addSend(std::unique_ptr<Send> send);

    const std::vector<std::unique_ptr<Send>>& getSends();

    /// Make this track output to the defined track
    void setOutput(std::weak_ptr<Track> newOutputTrack);

    String getId() {
        return id;
    }

    std::weak_ptr<Track> getOutput() {
        return outputTrack;
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
    float height;
    String name;
    juce::Colour color;
};
