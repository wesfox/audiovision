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

    pluginEditorHost = std::make_unique<PluginEditorHost>();
    addAndMakeVisible(pluginEditorHost.get());

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
        auto node = audioEngine->getPluginNode(reverbTrackId, "MReverbMB");
        if (node != nullptr) {
            pluginEditorHost->setPluginNode(node);
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
    if (pluginEditorHost != nullptr) {
        pluginEditorHost->setPluginNode(nullptr);
    }
    pluginEditorHost.reset();
    if (audioEngine != nullptr) {
        audioEngine->shutdown();
    }
    audioEngine.reset();
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();
    if (pluginEditorHost != nullptr) {
        auto editorBounds = bounds.removeFromRight(bounds.getWidth() / 2);
        pluginEditorHost->setBounds(editorBounds);
    }
    if (fileSelector != nullptr) {
        fileSelector->setBounds(bounds);
    }
}
