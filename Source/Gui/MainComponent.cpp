#include "MainComponent.h"

#include "Core/CustomEdits/EditTest.h"
#include "Core/Edit/Edit.h"

//==============================================================================
MainComponent::MainComponent()
{
    setSize (600, 400);

    edit = std::make_unique<EditTest>();

    fileSelector = std::make_unique<FileSelectorComponent>();
    fileSelector->onPlaySession = [this](const FileSelectorComponent::ImportedTrack& track) {
        transport.play();
    };
    addAndMakeVisible(fileSelector.get());

    audioOutputComponent = std::make_unique<AudioOutputComponent>(edit->getAudioOutputEngine());
    addAndMakeVisible(audioOutputComponent.get());

    // Test sound stuff
    auto graphManager = std::make_unique<GraphManager>(edit->getAudioGraph());
    graphManager->createGraph(*edit);
    graphManager->createFinalGraph();
    graphManager->attachAudioOutput(edit->getAudioOutputTrack());
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
    if (audioOutputComponent != nullptr) {
        auto header = bounds.removeFromTop(40);
        audioOutputComponent->setBounds(header);
    }
    if (fileSelector != nullptr) {
        fileSelector->setBounds(bounds);
    }
}
