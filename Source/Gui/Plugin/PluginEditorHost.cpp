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

bool PluginEditorHost::getEditorSize(int& width, int& height) const
{
    if (editor == nullptr) {
        return false;
    }
    width = editor->getWidth();
    height = editor->getHeight();
    if (width <= 0 || height <= 0) {
        auto bounds = editor->getBounds();
        width = bounds.getWidth();
        height = bounds.getHeight();
    }
    return width > 0 && height > 0;
}

void PluginEditorHost::resized()
{
    if (editor != nullptr) {
        editor->setBounds(getLocalBounds());
    }
}
