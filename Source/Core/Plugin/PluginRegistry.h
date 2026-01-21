#pragma once

#include <JuceHeader.h>

#include "Plugin.h"

class PluginRegistry {
public:
    void scan();

    const std::vector<std::shared_ptr<Plugin>>& getPlugins() const { return plugins; }
    std::shared_ptr<Plugin> getByName(const String& name) const;

private:
    std::vector<std::shared_ptr<Plugin>> plugins;
};
