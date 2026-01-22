#include "ValueTreeManager.h"

#include "AudioEngine/Graph/Runtime/GraphManager.h"

ValueTreeManager::ParameterHost::ParameterHost()
{
}

ValueTreeManager::ValueTreeManager() = default;

void ValueTreeManager::buildForGraph(const GraphManager& graphManager)
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    for (const auto& node : graphManager.graphNodes) {
        const auto descriptors = factory.collectForNode(*node);
        for (const auto& descriptor : descriptors) {
            const auto paramId = makeParamId(node->getTrackId(), descriptor.name);
            const juce::ParameterID parameterId(paramId, 1);
            juce::NormalisableRange<float> range(descriptor.minValue, descriptor.maxValue);
            auto parameter = std::make_unique<juce::AudioParameterFloat>(
                parameterId,
                descriptor.name,
                range,
                descriptor.defaultValue,
                descriptor.label);
            layout.add(std::move(parameter));
        }
    }

    apvts = std::make_unique<juce::AudioProcessorValueTreeState>(
        host,
        nullptr,
        juce::Identifier("AV_Parameters"),
        std::move(layout));
}

std::atomic<float>* ValueTreeManager::getRawParameterValue(const juce::String& trackId,
                                                           const juce::String& paramName) const
{
    if (!apvts) {
        return nullptr;
    }

    return apvts->getRawParameterValue(makeParamId(trackId, paramName));
}

juce::String ValueTreeManager::makeParamId(const juce::String& trackId,
                                           const juce::String& paramName) const
{
    const auto trackHash = juce::String::toHexString(trackId.hashCode64());
    return trackHash + ":" + paramName;
}
