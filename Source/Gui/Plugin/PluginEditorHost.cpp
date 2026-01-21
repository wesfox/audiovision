#include "PluginEditorHost.h"

PluginEditorHost::PluginEditorHost() = default;

PluginEditorHost::~PluginEditorHost() = default;

void PluginEditorHost::setPluginNode(juce::AudioProcessorGraph::Node::Ptr node)
{
    pluginNode = std::move(node);
    editor.reset();

    if (pluginNode == nullptr) {
        return;
    }

    auto* processor = pluginNode->getProcessor();
    if (processor == nullptr || !processor->hasEditor()) {
        return;
    }

    if (auto* createdEditor = processor->createEditor()) {
        editor.reset(createdEditor);
        addAndMakeVisible(editor.get());
        resized();
    }
}

void PluginEditorHost::resized()
{
    if (editor != nullptr) {
        editor->setBounds(getLocalBounds());
    }
}
