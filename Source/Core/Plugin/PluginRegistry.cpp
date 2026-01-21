#include "PluginRegistry.h"

void PluginRegistry::scan()
{
    plugins.clear();
    const juce::File vst3Root("/Library/Audio/Plug-Ins/VST3");
    if (!vst3Root.exists()) {
        juce::Logger::writeToLog("VST3 directory not found: " + vst3Root.getFullPathName());
        return;
    }

    juce::Array<juce::File> vst3Files;
    vst3Root.findChildFiles(vst3Files, juce::File::findDirectories, true, "*.vst3");

    std::set<String> seenPaths;
    for (const auto& file : vst3Files) {
        auto name = file.getFileNameWithoutExtension();
        auto path = file.getFullPathName();
        if (!seenPaths.insert(path).second) {
            continue;
        }
        plugins.push_back(std::make_shared<Plugin>(
            name,
            "Unknown",
            path,
            PluginFormat::VST3));
        juce::Logger::writeToLog("VST3 plugin: " + name + " (" + path + ")");
    }
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
