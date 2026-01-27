#include "AudioEngine.h"

#include "Core/Edit/Edit.h"
#include "Core/Plugin/PluginRegistry.h"

// ------------------------ MainComponent Implementation ------------------------

AudioEngine::AudioEngine(const std::weak_ptr<Edit>& edit):edit(edit) {
    if (auto editPtr = edit.lock()) {
        transport = editPtr->getTransport();
    }
    if (!transport) {
        transport = std::make_shared<Transport>();
    }

    if (auto editPtr = edit.lock()) {
        auto graphInstance = std::make_unique<GraphInstance>(editPtr, transport);
        graphInstance->build();
        graphInstances.emplace_back(std::move(graphInstance));
    }

    pluginHost = std::make_unique<PluginInstanceFactory>();
    pluginRegistry = std::make_unique<PluginRegistry>();
    pluginRegistry->scan();

    audioOutputEngine = std::make_unique<AudioOutputEngine>(graphInstances, transport);
    audioOutputEngine->initialise(ChannelsFormat::Mono, 48000.0, 512);
}

AudioEngine::~AudioEngine()
{
    shutdown();
}

void AudioEngine::start()
{
    transport->prepare(48000.0);
    transport->rewind();
    transport->play();
}

void AudioEngine::startRecording()
{
    for (auto& instance : graphInstances) {
        if (instance) {
            instance->startRecording();
        }
    }
}

void AudioEngine::stopRecording()
{
    for (auto& instance : graphInstances) {
        if (instance) {
            instance->stopRecording();
        }
    }
}

void AudioEngine::shutdown()
{
    if (audioOutputEngine) {
        audioOutputEngine->shutdown();
    }

    for (auto& instance : graphInstances) {
        if (instance) {
            instance->shutdown();
        }
    }
    graphInstances.clear();
    pluginRegistry.reset();
    pluginHost.reset();
}

juce::AudioProcessorGraph::Node::Ptr AudioEngine::getPluginNode(const String& trackId,
                                                                const String& pluginName) const
{
    for (const auto& instance : graphInstances) {
        auto node = instance->getGraphManager().findPluginNode(trackId, pluginName);
        if (node != nullptr) {
            return node;
        }
    }

    return {};
}

juce::AudioProcessorGraph::Node::Ptr AudioEngine::getPluginNodeByName(const String& pluginName) const
{
    for (const auto& instance : graphInstances) {
        auto node = instance->getGraphManager().findPluginNodeByName(pluginName);
        if (node != nullptr) {
            return node;
        }
    }

    return {};
}

const PluginInstanceStore* AudioEngine::getPluginInstanceStore() const
{
    if (graphInstances.empty() || !graphInstances.front()) {
        return nullptr;
    }
    return &graphInstances.front()->getGraphManager().getPluginInstanceStore();
}
