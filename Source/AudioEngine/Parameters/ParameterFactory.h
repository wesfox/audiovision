#pragma once

#include <JuceHeader.h>

#include <vector>

/// App-owned parameter identifiers.
enum class ParameterKey {
    Volume,
    StereoPan
};

/// Definition for an app-owned parameter.
struct ParamDef {
    ParameterKey key;
    juce::String name;
    float minValue;
    float maxValue;
    float defaultValue;
    juce::String label;
};

/// Return all app-owned parameter definitions.
const std::vector<ParamDef>& allParamDefs();

/// Find a parameter definition by key.
/// @param key parameter identifier
const ParamDef* findParamDef(ParameterKey key);

/// Get a parameter name by key.
/// @param key parameter identifier
juce::String paramName(ParameterKey key);
