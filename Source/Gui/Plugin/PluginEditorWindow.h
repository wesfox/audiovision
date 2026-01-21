#pragma once

#include <JuceHeader.h>

class PluginEditorHost;

class PluginEditorWindow : public juce::DocumentWindow {
public:
    PluginEditorWindow();

    void setPluginNode(juce::AudioProcessorGraph::Node::Ptr node);
    void closeButtonPressed() override;

private:
    void applyEditorSize();

    PluginEditorHost* pluginEditorHost = nullptr;
};
