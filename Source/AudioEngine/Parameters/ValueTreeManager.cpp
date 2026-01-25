#include "ValueTreeManager.h"

#include "AudioEngine/Graph/Runtime/GraphManager.h"

#include <set>

ValueTreeManager::ParameterHost::ParameterHost()
{
}

ValueTreeManager::ValueTreeManager() = default;

void ValueTreeManager::buildForGraph(const GraphManager& graphManager)
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    for (const auto& node : graphManager.graphNodes) {
        const auto& defs = allParamDefs();
        for (const auto& def : defs) {
            const auto paramId = makeParamId(node->getTrackId(), def.name);
            const juce::ParameterID parameterId(paramId, 1);
            juce::NormalisableRange<float> range(def.minValue, def.maxValue);
            auto parameter = std::make_unique<juce::AudioParameterFloat>(
                parameterId,
                def.name,
                range,
                def.defaultValue,
                def.label);
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

std::atomic<float>* ValueTreeManager::getRawParameterValue(const juce::String& trackId,
                                                           ParameterKey key) const
{
    return getRawParameterValue(trackId, paramName(key));
}

std::map<ParameterKey, std::atomic<float>*> ValueTreeManager::buildParamMap(
    const juce::String& trackId,
    const std::vector<ParameterKey>& keys) const
{
    std::map<ParameterKey, std::atomic<float>*> params;
    for (const auto key : keys) {
        params[key] = getRawParameterValue(trackId, key);
    }
    return params;
}

float ValueTreeManager::getParameterValue(const juce::String& trackId, ParameterKey key) const
{
    if (!apvts) {
        return 0.0f;
    }
    const auto paramId = makeParamId(trackId, key);
    auto* param = dynamic_cast<juce::RangedAudioParameter*>(apvts->getParameter(paramId));
    if (!param) {
        return 0.0f;
    }
    return param->convertFrom0to1(param->getValue());
}

void ValueTreeManager::setParameterValue(const juce::String& trackId,
                                         ParameterKey key,
                                         float value) const
{
    if (!apvts) {
        return;
    }
    const auto paramId = makeParamId(trackId, key);
    auto* param = dynamic_cast<juce::RangedAudioParameter*>(apvts->getParameter(paramId));
    if (!param) {
        return;
    }
    param->setValueNotifyingHost(param->convertTo0to1(value));
}

void ValueTreeManager::registerPluginParameters(const juce::String& pluginId,
                                                 juce::AudioProcessor& processor)
{
    std::vector<PluginParamInfo> params;
    const auto& parameters = processor.getParameters();
    params.reserve(static_cast<size_t>(parameters.size()));

    std::set<juce::String> usedIds;
    for (int i = 0; i < parameters.size(); ++i) {
        auto* param = parameters.getUnchecked(i);
        if (param == nullptr) {
            continue;
        }
        auto* ranged = dynamic_cast<juce::RangedAudioParameter*>(param);
        auto id = ranged ? ranged->getParameterID() : juce::String();
        if (id.isEmpty()) {
            id = "p" + juce::String(i);
        }
        if (usedIds.find(id) != usedIds.end()) {
            id = id + "_" + juce::String(i);
        }
        usedIds.insert(id);

        PluginParamInfo info;
        info.id = id;
        info.name = param->getName(256);
        info.param = param;
        info.rangedParam = ranged;
        params.push_back(info);
    }

    pluginParams[pluginId] = std::move(params);
}

const std::vector<ValueTreeManager::PluginParamInfo>* ValueTreeManager::getPluginParameters(
    const juce::String& pluginId) const
{
    auto it = pluginParams.find(pluginId);
    if (it == pluginParams.end()) {
        return nullptr;
    }
    return &it->second;
}

float ValueTreeManager::getPluginParameterValue(const juce::String& pluginId,
                                                const juce::String& paramId) const
{
    auto it = pluginParams.find(pluginId);
    if (it == pluginParams.end()) {
        return 0.0f;
    }
    for (const auto& info : it->second) {
        if (info.id == paramId && info.param != nullptr) {
            const auto value01 = info.param->getValue();
            if (info.rangedParam) {
                return info.rangedParam->convertFrom0to1(value01);
            }
            return value01;
        }
    }
    return 0.0f;
}

void ValueTreeManager::setPluginParameterValue(const juce::String& pluginId,
                                               const juce::String& paramId,
                                               float value) const
{
    auto it = pluginParams.find(pluginId);
    if (it == pluginParams.end()) {
        return;
    }
    for (const auto& info : it->second) {
        if (info.id == paramId && info.param != nullptr) {
            if (info.rangedParam) {
                info.param->setValueNotifyingHost(info.rangedParam->convertTo0to1(value));
            } else {
                info.param->setValueNotifyingHost(value);
            }
            return;
        }
    }
}

juce::String ValueTreeManager::makeParamId(const juce::String& trackId,
                                           const juce::String& paramName) const
{
    const auto trackHash = juce::String::toHexString(trackId.hashCode64());
    return trackHash + ":" + paramName;
}

juce::String ValueTreeManager::makeParamId(const juce::String& trackId,
                                           ParameterKey key) const
{
    return makeParamId(trackId, paramName(key));
}
