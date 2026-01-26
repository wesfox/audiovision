#pragma once

#include <JuceHeader.h>

class PluginEditorHost;

/// Floating window that hosts a plugin editor UI.
class PluginEditorWindow : public juce::DocumentWindow {
public:
    /// Create the plugin editor window.
    PluginEditorWindow();

    /// Set the plugin node to display.
    /// @param node plugin graph node
    void setPluginNode(juce::AudioProcessorGraph::Node::Ptr node);

    /// Hide the window when closed.
    void closeButtonPressed() override;

private:
    /// Resize the window to match the plugin editor.
    void applyEditorSize();

    PluginEditorHost* pluginEditorHost = nullptr;
};
