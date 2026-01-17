#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    setSize (600, 400);

    fileSelector = std::make_unique<FileSelectorComponent>();
    fileSelector->onPlaySession = [this](const FileSelectorComponent::ImportedTrack& track) {
        transport.play();
    };
    addAndMakeVisible(fileSelector.get());

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
    if (fileSelector != nullptr)
        fileSelector->setBounds(getLocalBounds());
}
