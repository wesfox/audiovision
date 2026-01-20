#include "AudioTrackNode.h"

#include <Core/Track/AudioTrack.h>

#include <utility>

AudioTrackNode::AudioTrackNode(const std::weak_ptr<AudioTrack> &audioTrack, const Transport * transport):
    audioTrack(audioTrack.lock().get()), transport(transport)
{
    setPlayConfigDetails(2, 2, 48000.0, 512);
}

void AudioTrackNode::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    buffer.clear();

    for (const auto& audioClip : audioTrack->getAudioClips()) {
        auto audioClipStartSample = audioClip->getSessionStartSample();
        auto audioClipEndSample = audioClip->getSessionEndSample();
        if (audioClipStartSample <= transport->getCursorPosition() && transport->getCursorPosition() < audioClipEndSample) {
            juce::AudioBuffer<float> audioClipBuffer = audioClip->read( transport->getCursorPosition() - audioClipStartSample, buffer.getNumSamples());
            for (int c=0; c!= buffer.getNumChannels(); c++) {
                auto bufferChannel = buffer.getWritePointer(c);
                auto audioClipBufferReader = audioClipBuffer.getReadPointer(c ? c < audioClipBuffer.getNumChannels() : audioClipBuffer.getNumChannels() -1);
                for (int i=0; i!= buffer.getNumSamples(); i++) {
                    bufferChannel[i] += audioClipBufferReader[i];
                }
            }
        }
    }
}