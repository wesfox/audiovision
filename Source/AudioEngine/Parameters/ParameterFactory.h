#pragma once

#include <JuceHeader.h>

#include <vector>

enum class ParameterKey {
    Volume,
    StereoPan
};

struct ParamDef {
    ParameterKey key;
    juce::String name;
    float minValue;
    float maxValue;
    float defaultValue;
    juce::String label;
};

const std::vector<ParamDef>& allParamDefs();
const ParamDef* findParamDef(ParameterKey key);
juce::String paramName(ParameterKey key);
