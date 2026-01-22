#include "GraphInstance.h"

#include "Core/Edit/Edit.h"

GraphInstance::GraphInstance(const std::shared_ptr<Edit>& edit,
                             const std::shared_ptr<Transport>& transport,
                             std::weak_ptr<Scene> scene)
    :
        edit(edit),
        transport(transport),
        graph(std::make_shared<juce::AudioProcessorGraph>()),
        scene(std::move(scene))
{
    graphManager = std::make_unique<GraphManager>(edit, graph);
}

void GraphInstance::build() const {
    graphManager->createGraphFromScene(getScene());
    graphManager->createFinalGraph(transport);
    graphManager->attachAudioOutput(edit->getAudioOutputTrack());
    auto transportPtr = transport;
    auto sampleRate = transportPtr ? transportPtr->getSampleRate() : 48000.0;
    auto blockSize = transportPtr ? transportPtr->getCurrentBlockSize() : 512;
    graphManager->prepareToPlay(sampleRate, blockSize);
}

void GraphInstance::prepareToPlay(double sampleRate, int blockSize)
{
    graphManager->prepareToPlay(sampleRate, blockSize);
}

void GraphInstance::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    graph->processBlock(buffer, midi);
}

void GraphInstance::shutdown()
{
    graph->releaseResources();
    graph->clear();
    graphManager.reset();
}
