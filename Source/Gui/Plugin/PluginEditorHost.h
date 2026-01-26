#pragma once

#include <JuceHeader.h>

/// Host component for embedding a plugin editor UI.
class PluginEditorHost : public juce::Component {
public:
    /// Create the plugin editor host.
    PluginEditorHost();
    ~PluginEditorHost() override;

    /// Set the plugin node to display.
    /// @param node plugin graph node
    void setPluginNode(juce::AudioProcessorGraph::Node::Ptr node);

    /// Query the current editor size.
    /// @param width output width
    /// @param height output height
    bool getEditorSize(int& width, int& height) const;

    /// Layout the embedded editor.
    void resized() override;

private:
    juce::AudioProcessorGraph::Node::Ptr pluginNode;
    std::unique_ptr<juce::AudioProcessorEditor> editor;
};
