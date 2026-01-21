#include "PluginRegistry.h"

void PluginRegistry::scan()
{
    plugins.clear();
    plugins.push_back(std::make_shared<Plugin>(
        "TAL-Reverb",
        "TAL",
        "/Library/Audio/Plug-Ins/VST3/TAL-Reverb-2.vst3",
        PluginFormat::VST3));
    plugins.push_back(std::make_shared<Plugin>(
        "Another VST3",
        "Vendor",
        "/Library/Audio/Plug-Ins/VST3/Another.vst3",
        PluginFormat::VST3));
}

std::shared_ptr<Plugin> PluginRegistry::getByName(const String& name) const
{
    for (const auto& plugin : plugins) {
        if (plugin->getName() == name) {
            return plugin;
        }
    }
    return {};
}
