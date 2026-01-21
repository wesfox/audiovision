#pragma once

#include <JuceHeader.h>

class PluginEditorHost : public juce::Component {
public:
    PluginEditorHost();
    ~PluginEditorHost() override;

    void setPluginNode(juce::AudioProcessorGraph::Node::Ptr node);
    bool getEditorSize(int& width, int& height) const;

    void resized() override;

private:
    juce::AudioProcessorGraph::Node::Ptr pluginNode;
    std::unique_ptr<juce::AudioProcessorEditor> editor;
};
