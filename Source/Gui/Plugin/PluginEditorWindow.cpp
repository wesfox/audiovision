#include "PluginEditorWindow.h"

#include "PluginEditorHost.h"

PluginEditorWindow::PluginEditorWindow()
    : DocumentWindow("Plugin",
                     juce::Desktop::getInstance().getDefaultLookAndFeel()
                         .findColour(juce::ResizableWindow::backgroundColourId),
                     DocumentWindow::closeButton)
{
    setUsingNativeTitleBar(true);
    auto host = std::make_unique<PluginEditorHost>();
    pluginEditorHost = host.get();
    setContentOwned(host.release(), true);
    setResizable(true, true);
    centreWithSize(600, 400);
}

void PluginEditorWindow::setPluginNode(juce::AudioProcessorGraph::Node::Ptr node)
{
    if (pluginEditorHost != nullptr) {
        pluginEditorHost->setPluginNode(std::move(node));
        applyEditorSize();
        juce::MessageManager::callAsync([this]() {
            applyEditorSize();
        });
    }
}

void PluginEditorWindow::closeButtonPressed()
{
    setVisible(false);
}

void PluginEditorWindow::applyEditorSize()
{
    if (pluginEditorHost == nullptr) {
        return;
    }
    int width = 0;
    int height = 0;
    if (!pluginEditorHost->getEditorSize(width, height)) {
        return;
    }
    setResizable(false, false);
    pluginEditorHost->setSize(width, height);
    setSize(width, height + getTitleBarHeight());
    centreWithSize(getWidth(), getHeight());
    if (auto* editor = pluginEditorHost->getChildComponent(0)) {
        editor->setBounds(0, 0, width, height);
    }
    setResizable(true, true);
}
