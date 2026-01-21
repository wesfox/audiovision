#include "AudioEngine.h"

#include "Core/Edit/Edit.h"
#include "Graph/Runtime/GraphManager.h"

// ------------------------ MainComponent Implementation ------------------------

AudioEngine::AudioEngine(const std::weak_ptr<Edit>& edit):edit(edit) {
    transport = std::make_shared<Transport>();

    if (auto editPtr = edit.lock()) {
        for (int i =0; i!=editPtr->getScenes().size(); i++) {
            auto graphInstance = std::make_unique<GraphInstance>(editPtr, transport, i);
            graphInstance->build();
            graphInstances.emplace_back(std::move(graphInstance));
        }

    }

    audioOutputEngine = std::make_unique<AudioOutputEngine>(graphInstances, transport);
    audioOutputEngine->initialise(ChannelsFormat::Mono, 48000.0, 512);
}

void AudioEngine::start()
{
    transport->prepare(48000.0);
    transport->rewind();
    transport->play();
}
