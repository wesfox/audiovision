#include "ParameterFactory.h"

const std::vector<ParamDef>& allParamDefs()
{
    static const std::vector<ParamDef> defs = {
        { ParameterKey::Volume, "volume", 0.0f, 1.0f, 1.0f, "" },
        { ParameterKey::StereoPan, "stereoPan", -1.0f, 1.0f, 0.0f, "" }
    };
    return defs;
}

const ParamDef* findParamDef(ParameterKey key)
{
    const auto& defs = allParamDefs();
    for (const auto& def : defs) {
        if (def.key == key) {
            return &def;
        }
    }
    return nullptr;
}

juce::String paramName(ParameterKey key)
{
    if (const auto* def = findParamDef(key)) {
        return def->name;
    }
    return "unknown";
}
