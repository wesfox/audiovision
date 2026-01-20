#include "MainComponent.h"

#include "AudioEngine/AudioEngine.h"
#include "AudioEngine/Graph/GraphManager.h"
#include "Core/CustomEdits/EditTest.h"

//==============================================================================
MainComponent::MainComponent()
{
    setSize (600, 400);

    edit = std::make_shared<EditTest>();

    fileSelector = std::make_unique<FileSelectorComponent>();
    fileSelector->onPlaySession = [this](const FileSelectorComponent::ImportedTrack& track) {
        transport.play();
    };
    addAndMakeVisible(fileSelector.get());

    // Test sound stuff
    audioEngine = std::make_unique<AudioEngine>(edit);
    audioEngine->start();

    // end test sound stuff

    setSize(800, 700);
}

MainComponent::~MainComponent()
{
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
