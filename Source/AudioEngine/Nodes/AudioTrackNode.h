#pragma once

#include <JuceHeader.h>

#include "Core/Track/AudioTrack.h"
#include "Utils/Transport.h"

class GraphNode;

class AudioTrackNode : public juce::AudioProcessor {
public:
    AudioTrackNode(const std::weak_ptr<AudioTrack>& audioTrack,
                   const std::weak_ptr<Transport>& transport,
                   const GraphNode* graphNode);

    const juce::String getName() const override;

    void prepareToPlay(double, int) override {}
    void releaseResources() override {}

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override { return true; }


    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override;

    // Obligatoire
    double getTailLengthSeconds() const override { return 0.0; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override {}
    void setStateInformation(const void*, int) override {}
    bool hasEditor() const override { return false; }
    AudioProcessorEditor* createEditor() override { return nullptr; }

private:
    std::weak_ptr<Transport> transport;
    std::weak_ptr<AudioTrack> audioTrack;
    const GraphNode* graphNode;
};
