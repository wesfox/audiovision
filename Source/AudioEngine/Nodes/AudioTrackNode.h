#pragma once

#include "AudioNode.h"

#include <atomic>
#include "Core/Track/AudioTrack.h"
#include "Utils/Transport.h"

class GraphNode;
class Recorder;

/// Runtime node that renders audio clips from an AudioTrack.
class AudioTrackNode : public AudioNode {
public:
    /// Create a node bound to a track and transport.
    /// @param audioTrack track to render
    /// @param transport timeline transport
    /// @param graphNode owning graph node
    AudioTrackNode(const std::weak_ptr<AudioTrack>& audioTrack,
                   const std::weak_ptr<Transport>& transport,
                   const GraphNode* graphNode);

    /// Display name for debugging/graph views.
    const juce::String getName() const override;

    /// Render track clips into the buffer.
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override;

    /// Attach a recorder for the next record pass.
    /// @param recorder recorder instance to use
    void setActiveRecorder(Recorder* recorder);

    /// True when the underlying track is armed for recording.
    bool isTrackArmed() const;

    /// Access the underlying audio track (may be null).
    std::shared_ptr<AudioTrack> getTrack() const;

private:
    std::weak_ptr<Transport> transport;
    std::weak_ptr<AudioTrack> audioTrack;
    const GraphNode* graphNode;
    std::atomic<Recorder*> activeRecorder{ nullptr };
};
