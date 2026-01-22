#include "GraphInstance.h"

#include "Core/Edit/Edit.h"

GraphInstance::GraphInstance(const std::shared_ptr<Edit>& edit,
                             const std::shared_ptr<Transport>& transport)
    :
        edit(edit),
        transport(transport),
        graph(std::make_shared<juce::AudioProcessorGraph>()),
        recordSession(std::make_unique<RecordSession>())
{
    graphManager = std::make_unique<GraphManager>(edit, graph, recordSession.get());
}

void GraphInstance::build() const {
    graphManager->createGraphFromEdit();
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
    if (graphManager) {
        graphManager->shutdown();
    }
    if (graph) {
        graph->releaseResources();
        graph->clear();
    }
    graphManager.reset();
    recordSession.reset();
}

void GraphInstance::startRecording()
{
    if (!recordSession) {
        return;
    }
    const auto transportPtr = transport;
    const auto startSample = transportPtr ? transportPtr->getCursorPosition() : 0;
    const auto sampleRate = transportPtr ? transportPtr->getSampleRate() : 48000.0;
    const auto blockSize = transportPtr ? transportPtr->getCurrentBlockSize() : 512;
    recordSession->begin(startSample, sampleRate, blockSize);
}

void GraphInstance::stopRecording()
{
    if (!recordSession) {
        return;
    }
    const auto transportPtr = transport;
    const auto endSample = transportPtr ? transportPtr->getCursorPosition() : 0;
    recordSession->end(endSample);
}
