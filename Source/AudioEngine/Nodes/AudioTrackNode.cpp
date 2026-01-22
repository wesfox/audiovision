#include "AudioTrackNode.h"

#include "AudioEngine/Graph/Model/GraphNode.h"
#include "AudioEngine/Recording/Recorder.h"

#include <Core/Track/AudioTrack.h>

#include <utility>

AudioTrackNode::AudioTrackNode(const std::weak_ptr<AudioTrack>& audioTrack,
                               const std::weak_ptr<Transport>& transport,
                               const GraphNode* graphNode)
    : transport(transport),
      audioTrack(audioTrack),
      graphNode(graphNode)
{
    setPlayConfigDetails(2, 2, 48000.0, 512);
}

const juce::String AudioTrackNode::getName() const
{
    if (graphNode != nullptr) {
        return graphNode->getName() + ":AudioTrackNode";;
    }
    return "NoName:AudioTrackNode";
}

void AudioTrackNode::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    const auto transportPtr = transport.lock();
    const auto trackPtr = audioTrack.lock();
    if (!transportPtr || !trackPtr) {
        return;
    }

    if (auto* recorder = activeRecorder.load(std::memory_order_relaxed)) {
        recorder->pushBuffer(buffer, buffer.getNumSamples());
    }

    for (const auto& audioClip : trackPtr->getAudioClips()) {
        // define all local variable needed to do easy to read computation
        auto currentCursorPosition = transportPtr->getCursorPosition();
        auto audioClipStartSample = audioClip->getSessionStartSample();
        auto audioClipEndSample = audioClip->getSessionEndSample();
        int64 numSamples = buffer.getNumSamples();


        // position of start and end to write in the buffer
        auto writeStartSample = std::max(0LL, audioClipStartSample - currentCursorPosition);
        auto writeEndSample = std::min(numSamples, audioClipEndSample - currentCursorPosition);

        if (audioClipStartSample <= currentCursorPosition + numSamples && currentCursorPosition < audioClipEndSample) {
            juce::AudioBuffer<float> audioClipBuffer = audioClip->read( currentCursorPosition - audioClipStartSample, buffer.getNumSamples());
            for (int c=0; c!= buffer.getNumChannels(); c++) {
                auto audioClipChannel = std::min(c, audioClipBuffer.getNumChannels() - 1);
                for (int i=writeStartSample; i < writeEndSample; i++) {
                    buffer.addSample(c, i, audioClipBuffer.getSample(audioClipChannel,i));
                }
            }
        }
    }
}

void AudioTrackNode::setActiveRecorder(Recorder* recorder)
{
    activeRecorder.store(recorder, std::memory_order_relaxed);
}

bool AudioTrackNode::isTrackArmed() const
{
    if (const auto trackPtr = audioTrack.lock()) {
        return trackPtr->isArmed();
    }
    return false;
}

std::shared_ptr<AudioTrack> AudioTrackNode::getTrack() const
{
    return audioTrack.lock();
}

// void AudioTrackNode::applyGain() {
//     auto startGain = float(transport->getCursorPosition() - audioClipStartSample)/float(audioClipEndSample - audioClipStartSample);
//     auto endGain = float(transport->getCursorPosition() + numSamples - audioClipStartSample)/float(audioClipEndSample - audioClipStartSample);
//     buffer.applyGainRamp(c,0,buffer.getNumSamples(), startGain, endGain);
// }
