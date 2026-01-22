#include "RecordSession.h"

#include "AudioEngine/Nodes/AudioTrackNode.h"
#include "AudioEngine/Recording/Recorder.h"
#include "Core/AudioClip/AudioClip.h"
#include "Core/Track/AudioTrack.h"
#include "Utils/IO/AudioFile.h"

RecordSession::RecordSession(int maxRecordSeconds)
    : maxRecordSeconds(maxRecordSeconds)
{
}

void RecordSession::registerTrackNode(const String& trackId, AudioTrackNode* node)
{
    if (node == nullptr) {
        return;
    }
    trackNodes[trackId] = node;
}

void RecordSession::unregisterTrackNode(const String& trackId)
{
    trackNodes.erase(trackId);
}

void RecordSession::clearTrackNodes()
{
    trackNodes.clear();
}

void RecordSession::begin(int64 startSample, double newSampleRate, int newBlockSize)
{
    state.isRecording = true;
    state.startSample = startSample;
    state.endSample = startSample;
    sampleRate = newSampleRate;
    blockSize = newBlockSize;
    activeRecorders.clear();
    juce::Logger::writeToLog("RecordSession::begin at sample " + juce::String(startSample));

    const int64 maxSamples = static_cast<int64>(maxRecordSeconds * sampleRate);
    for (const auto& [trackId, node] : trackNodes) {
        if (node == nullptr || !node->isTrackArmed()) {
            if (node != nullptr) {
                node->setActiveRecorder(nullptr);
            }
            continue;
        }
        const auto track = node->getTrack();
        if (!track) {
            node->setActiveRecorder(nullptr);
            continue;
        }
        const int channels = std::max(1, node->getTotalNumOutputChannels());
        auto recorder = std::make_unique<Recorder>(channels, maxSamples, sampleRate);
        auto* recorderPtr = recorder.get();
        activeRecorders.push_back({ trackId, track, std::move(recorder) });
        node->setActiveRecorder(recorderPtr);
        juce::Logger::writeToLog("RecordSession::begin armed track " + trackId);
    }
}

void RecordSession::end(int64 endSample)
{
    state.isRecording = false;
    state.endSample = endSample;
    juce::Logger::writeToLog("RecordSession::end at sample " + juce::String(endSample));

    for (const auto& [trackId, node] : trackNodes) {
        if (node != nullptr) {
            node->setActiveRecorder(nullptr);
        }
    }

    for (const auto& activeRecorder : activeRecorders) {
        finalizeRecorder(activeRecorder);
    }
    activeRecorders.clear();
}

juce::File RecordSession::recordingsDirectory() const
{
    auto dir = juce::File::getCurrentWorkingDirectory().getChildFile("recordings");
    if (!dir.exists()) {
        dir.createDirectory();
    }
    return dir;
}

void RecordSession::finalizeRecorder(const ActiveRecorder& activeRecorder)
{
    auto track = activeRecorder.track.lock();
    if (!track || !activeRecorder.recorder) {
        return;
    }
    const auto recordedSamples = activeRecorder.recorder->getNumSamplesRecorded();
    if (recordedSamples <= 0) {
        juce::Logger::writeToLog("RecordSession::finalizeRecorder no samples for " + activeRecorder.trackId);
        return;
    }

    auto fileName = activeRecorder.trackId + "_" + juce::String(state.startSample) + ".wav";
    auto file = recordingsDirectory().getChildFile(fileName);
    activeRecorder.recorder->writeToFile(file);
    juce::Logger::writeToLog("RecordSession::finalizeRecorder wrote " + file.getFullPathName());

    auto audioFile = AudioFile::get(file);
    auto clip = AudioClip::create(audioFile,
                                  0,
                                  state.startSample,
                                  state.startSample + recordedSamples);
    track->addAudioClip(std::move(clip));
    juce::Logger::writeToLog("RecordSession::finalizeRecorder clip added to " + activeRecorder.trackId);
}
