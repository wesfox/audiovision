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

    recordComponent = std::make_unique<RecordComponent>();
    recordComponent->onStartRecording = [this]() {
        if (audioEngine != nullptr) {
            audioEngine->startRecording();
        }
    };
    recordComponent->onStopRecording = [this]() {
        if (audioEngine != nullptr) {
            audioEngine->stopRecording();
        }
    };
    addAndMakeVisible(recordComponent.get());

    pluginEditorWindow = std::make_unique<PluginEditorWindow>();

    // Test sound stuff
    audioEngine = std::make_unique<AudioEngine>(edit);
    audioEngine->start();

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
    recordComponent.reset();
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
    auto controlsArea = bounds.removeFromTop(40);
    if (recordComponent != nullptr) {
        recordComponent->setBounds(controlsArea);
    }
    if (fileSelector != nullptr) {
        fileSelector->setBounds(bounds);
    }
}
