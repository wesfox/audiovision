#pragma once

#include "AudioNode.h"

#include <atomic>
#include "Core/Track/AudioTrack.h"
#include "Utils/Transport.h"

class GraphNode;
class Recorder;

class AudioTrackNode : public AudioNode {
public:
    AudioTrackNode(const std::weak_ptr<AudioTrack>& audioTrack,
                   const std::weak_ptr<Transport>& transport,
                   const GraphNode* graphNode);

    const juce::String getName() const override;

    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override;
    void setActiveRecorder(Recorder* recorder);
    bool isTrackArmed() const;
    std::shared_ptr<AudioTrack> getTrack() const;

private:
    std::weak_ptr<Transport> transport;
    std::weak_ptr<AudioTrack> audioTrack;
    const GraphNode* graphNode;
    std::atomic<Recorder*> activeRecorder{ nullptr };
};
