#pragma once

#include <JuceHeader.h>

#include "Plugin.h"

/// Registry of available plugins on the system.
class PluginRegistry {
public:
    /// Scan the system for available plugins.
    void scan();

    /// Access all registered plugins.
    const std::vector<std::shared_ptr<Plugin>>& getPlugins() const { return plugins; }

    /// Find a plugin by name.
    /// @param name plugin display name
    std::shared_ptr<Plugin> getByName(const String& name) const;

private:
    std::vector<std::shared_ptr<Plugin>> plugins;
};
