#include "AudioTrackNode.h"

#include <Core/Track/AudioTrack.h>

AudioTrackNode::AudioTrackNode()
{
    setPlayConfigDetails(2, 2, 48000.0, 512);
}

void AudioTrackNode::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    // if (audioTrack)
    //     audioTrack->render(buffer, buffer.getNumSamples());
}