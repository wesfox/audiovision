#include "MainComponent.h"

#include "AudioEngine/AudioEngine.h"
#include "Core/CustomEdits/ImportEdit.h"
#include "Utils/IO/EditSerializer.h"
#include "Gui/Style/Font.h"

//==============================================================================
MainComponent::MainComponent()
{
    setSize (600, 400);

    edit = ImportEdit::importEdit();

    const auto baseFont = Fonts::p(14.0f, Fonts::Weight::Regular);
    juce::LookAndFeel::getDefaultLookAndFeel().setDefaultSansSerifTypeface(baseFont.getTypefacePtr());
    audioEngine = std::make_unique<AudioEngine>(edit);

    header = std::make_unique<Header>(*edit);
    addAndMakeVisible(header.get());

    contextualSection = std::make_unique<ContextualSection>();
    addAndMakeVisible(contextualSection.get());

    trackHeaderPanel = std::make_unique<TrackHeaderPanel>(*edit);
    addAndMakeVisible(trackHeaderPanel.get());

    trackContentPanel = std::make_unique<TrackContentPanel>(*edit);
    addAndMakeVisible(trackContentPanel.get());

    setSize(1080/9*16, 700);
}

MainComponent::~MainComponent()
{
    shutdown();
}

void MainComponent::shutdown()
{
    EditSerializer::exportToFile(*edit, juce::File("/Users/nico/code/Audiovision/exports"));
    if (isShutDown) {
        return;
    }
    isShutDown = true;
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
    g.fillAll(juce::Colour::fromString("#FF332F45"));
}

void MainComponent::resized()
{
    constexpr int padding = 8;
    auto bounds = getLocalBounds().reduced(padding);
    auto contextualBounds = bounds.removeFromRight(300);
    bounds.removeFromRight(padding);
    const auto headerHeight = edit != nullptr ? edit->getState().getHeaderHeight() : 90;
    auto headerArea = bounds.removeFromTop(headerHeight);
    bounds.removeFromTop(padding);
    if (contextualSection != nullptr) {
        contextualSection->setBounds(contextualBounds);
    }
    if (header != nullptr) {
        header->setBounds(headerArea);
    }
    if (trackHeaderPanel != nullptr) {
        trackHeaderPanel->setBounds(bounds.removeFromLeft(200));
    }
    if (trackContentPanel !=nullptr) {
        trackContentPanel->setBounds(bounds);
    }
}
