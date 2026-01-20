#include "AudioEngine.h"

#include "Core/Edit/Edit.h"
#include "Graph/GraphManager.h"

// ------------------------ MainComponent Implementation ------------------------

AudioEngine::AudioEngine(const std::weak_ptr<Edit>& edit):edit(edit) {
    transport = std::make_shared<Transport>();
    audioGraph = std::make_shared<juce::AudioProcessorGraph>();

    graphManager = std::make_unique<GraphManager>(edit, this);
    graphManager->createGraph();
    graphManager->createFinalGraph(transport);
    graphManager->attachAudioOutput(edit.lock()->getAudioOutputTrack());
    graphManager->prepareToPlay();

    audioOutputEngine = std::make_unique<AudioOutputEngine>(audioGraph, this);
    audioOutputEngine->initialise(ChannelsFormat::Mono, 48000.0, 512);
}

void AudioEngine::start()
{
    transport->prepare(48000.0);
    transport->rewind();
    transport->play();
}
