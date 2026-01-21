#include "MainComponent.h"

#include "AudioEngine/AudioEngine.h"
#include "Core/CustomEdits/EditTest.h"

//==============================================================================
MainComponent::MainComponent()
{
    setSize (600, 400);

    edit = std::make_shared<EditTest>();

    fileSelector = std::make_unique<FileSelectorComponent>();
    fileSelector->onPlaySession = [this](const FileSelectorComponent::ImportedTrack& track) {
        audioEngine->transport->rewind();
        audioEngine->transport->play();
    };
    addAndMakeVisible(fileSelector.get());

    pluginEditorWindow = std::make_unique<PluginEditorWindow>();

    // Test sound stuff
    audioEngine = std::make_unique<AudioEngine>(edit);
    audioEngine->start();
    String reverbTrackId;
    for (const auto& track : edit->getTracks()) {
        if (track->getName() == "reverbAuxTrack") {
            reverbTrackId = track->getId();
            break;
        }
    }
    if (reverbTrackId.isNotEmpty()) {
        auto node = audioEngine->getPluginNode(reverbTrackId, "TAL-Reverb-2");
        if (node != nullptr) {
            pluginEditorWindow->setPluginNode(node);
            pluginEditorWindow->setVisible(true);
        } else {
            juce::Logger::writeToLog("Plugin node not found for reverbAuxTrack");
        }
    } else {
        juce::Logger::writeToLog("Reverb aux track not found");
    }

    // end test sound stuff

    setSize(800, 700);
}

MainComponent::~MainComponent()
{
    shutdown();
}

void MainComponent::shutdown()
{
    if (isShutDown) {
        return;
    }
    isShutDown = true;
    if (pluginEditorWindow != nullptr) {
        pluginEditorWindow->setPluginNode(nullptr);
        pluginEditorWindow->setVisible(false);
    }
    pluginEditorWindow.reset();
    if (audioEngine != nullptr) {
        audioEngine->shutdown();
    }
    audioEngine.reset();
    if (auto* messageManager = juce::MessageManager::getInstanceWithoutCreating()) {
        messageManager->runDispatchLoopUntil(50);
    }
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();
    if (fileSelector != nullptr) {
        fileSelector->setBounds(bounds);
    }
}
