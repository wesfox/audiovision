#include "PluginInstanceStore.h"

juce::AudioProcessorGraph::Node::Ptr PluginInstanceStore::find(const String& trackId,
                                                               const String& pluginName) const
{
    for (const auto& entry : pluginInstances) {
        if (entry.trackId == trackId && entry.pluginName == pluginName) {
            return entry.node;
        }
    }

    return {};
}

juce::AudioProcessorGraph::Node::Ptr PluginInstanceStore::findByPluginName(const String& pluginName) const
{
    for (const auto& entry : pluginInstances) {
        if (entry.pluginName == pluginName) {
            return entry.node;
        }
    }

    return {};
}

std::vector<PluginInstanceStore::ParamInfo> PluginInstanceStore::getParameters(const String& pluginName) const
{
    std::vector<ParamInfo> out;
    auto node = findByPluginName(pluginName);
    if (node == nullptr) {
        return out;
    }

    auto* processor = node->getProcessor();
    if (processor == nullptr) {
        return out;
    }

    const auto& params = processor->getParameters();
    out.reserve(static_cast<size_t>(params.size()));
    for (int i = 0; i < params.size(); ++i) {
        auto* param = params.getUnchecked(i);
        if (param == nullptr) {
            continue;
        }
        auto* ranged = dynamic_cast<juce::RangedAudioParameter*>(param);
        ParamInfo info;
        info.name = param->getName(256);
        info.id = ranged ? ranged->getParameterID() : juce::String();
        if (info.id.isEmpty()) {
            info.id = info.name.isNotEmpty() ? info.name : "p" + juce::String(i);
        }
        info.value = ranged ? ranged->convertFrom0to1(param->getValue()) : param->getValue();
        if (ranged) {
            const auto range = ranged->getNormalisableRange();
            info.minValue = range.start;
            info.maxValue = range.end;
            info.defaultValue = ranged->convertFrom0to1(ranged->getDefaultValue());
        } else {
            info.minValue = 0.0f;
            info.maxValue = 1.0f;
            info.defaultValue = 0.0f;
        }
        out.push_back(info);
    }

    return out;
}

PluginInstanceStore::ParamInfo PluginInstanceStore::getParameterByName(const String& pluginName,
                                                                       const String& paramName) const
{
    auto params = getParameters(pluginName);
    for (const auto& param : params) {
        if (param.name == paramName) {
            return param;
        }
    }
    return {};
}

float PluginInstanceStore::getParameterValue(const String& pluginName, const String& paramId) const
{
    auto node = findByPluginName(pluginName);
    if (node == nullptr) {
        return 0.0f;
    }
    auto* processor = node->getProcessor();
    if (processor == nullptr) {
        return 0.0f;
    }

    const auto& params = processor->getParameters();
    for (int i = 0; i < params.size(); ++i) {
        auto* param = params.getUnchecked(i);
        if (param == nullptr) {
            continue;
        }
        auto* ranged = dynamic_cast<juce::RangedAudioParameter*>(param);
        auto id = ranged ? ranged->getParameterID() : juce::String();
        const auto name = param->getName(256);
        if (id.isEmpty()) {
            id = name.isNotEmpty() ? name : "p" + juce::String(i);
        }
        if (id == paramId || name == paramId) {
            return ranged ? ranged->convertFrom0to1(param->getValue()) : param->getValue();
        }
    }
    return 0.0f;
}

bool PluginInstanceStore::setParameterValue(const String& pluginName,
                                            const String& paramId,
                                            float value) const
{
    auto node = findByPluginName(pluginName);
    if (node == nullptr) {
        return false;
    }
    auto* processor = node->getProcessor();
    if (processor == nullptr) {
        return false;
    }

    const auto& params = processor->getParameters();
    for (int i = 0; i < params.size(); ++i) {
        auto* param = params.getUnchecked(i);
        if (param == nullptr) {
            continue;
        }
        auto* ranged = dynamic_cast<juce::RangedAudioParameter*>(param);
        auto id = ranged ? ranged->getParameterID() : juce::String();
        const auto name = param->getName(256);
        if (id.isEmpty()) {
            id = name.isNotEmpty() ? name : "p" + juce::String(i);
        }
        if (id == paramId || name == paramId) {
            if (ranged) {
                param->setValueNotifyingHost(ranged->convertTo0to1(value));
            } else {
                param->setValueNotifyingHost(value);
            }
            return true;
        }
    }
    return false;
}
