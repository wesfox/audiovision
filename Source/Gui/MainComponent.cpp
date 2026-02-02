#include "MainComponent.h"

#include "AudioEngine/AudioEngine.h"
#include "Core/CustomEdits/ImportEdit.h"
#include "Utils/IO/EditSerializer.h"
#include "Utils/DebugWatchRegistry.h"
#include "Gui/Style/Font.h"
#include "Command/Commands.h"

//==============================================================================
MainComponent::MainComponent()
{
    setSize (600, 400);

    edit = ImportEdit::importEdit();
    commandCenter = std::make_unique<CommandCenter>(*edit);
    wheelCommandManager = std::make_unique<WheelCommandManager>(commandCenter->getCommandManager());
    selectionManager = std::make_unique<SelectionManager>(*edit);
    cursorController = std::make_unique<CursorController>(*edit, *selectionManager);
    selectionManager->setCursorController(cursorController.get());
    addKeyListener(&commandCenter->getKeyMappings());
    setWantsKeyboardFocus(true);
    grabKeyboardFocus();

    DebugWatchRegistry::get().setWatch("InsertionFollowsPlayback",
                                       &edit->getState(),
                                       [](const void* state) {
                                           const auto* editState = static_cast<const EditState*>(state);
                                           return juce::String(editState->getInsertionFollowsPlayback() ? "On" : "Off");
                                       });

    edit->getTransport()->setFrameRate(24);

    const auto baseFont = Fonts::p(14.0f, Fonts::Weight::Regular);
    juce::LookAndFeel::getDefaultLookAndFeel().setDefaultSansSerifTypeface(baseFont.getTypefacePtr());
    audioEngine = std::make_unique<AudioEngine>(edit);

    header = std::make_unique<Header>(*edit);
    addAndMakeVisible(header.get());

    contextualSection = std::make_unique<ContextualSection>();
    addAndMakeVisible(contextualSection.get());

    trackHeaderPanel = std::make_unique<TrackHeaderPanel>(*edit, *selectionManager);
    addAndMakeVisible(trackHeaderPanel.get());

    trackContentPanel = std::make_unique<TrackContentPanel>(*edit, *selectionManager);
    addAndMakeVisible(trackContentPanel.get());

    videoView = std::make_unique<VideoView>(*edit);
    videoWindow = std::make_unique<juce::DocumentWindow>(
        "Video",
        juce::Colours::black,
        juce::DocumentWindow::allButtons);
    videoWindow->setUsingNativeTitleBar(true);
    videoWindow->setResizable(true, true);
    videoView->onActiveClipChanged([this](const juce::String& name) {
        if (videoWindow != nullptr) {
            videoWindow->setName(name);
        }
    });
    videoWindow->setContentOwned(videoView.release(), true);
    const auto displays = juce::Desktop::getInstance().getDisplays();
    auto videoViewDisplay = displays.displays[displays.displays.size()-1];
    auto mainDisplay = std::ranges::find_if(displays.displays, [](const auto& d) {return d.isMain;});

    videoWindow->setBounds(videoViewDisplay.userArea);
    //videoWindow->setVisible(true);

    debugWatchWindow = std::make_unique<DebugWatchWindow>();
    if (mainDisplay != displays.displays.end()) {
        debugWatchWindow->setTopLeftPosition(mainDisplay->userArea.getX() + 20,
                                             mainDisplay->userArea.getY() + 20);
    }

    setSize(mainDisplay->userArea.getWidth(), mainDisplay->userArea.getHeight());
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
    if (videoWindow != nullptr) {
        videoWindow->setVisible(false);
        videoWindow.reset();
    }
    if (debugWatchWindow != nullptr) {
        debugWatchWindow->setVisible(false);
        debugWatchWindow.reset();
    }
    videoView.reset();
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

bool MainComponent::keyPressed(const juce::KeyPress& key) {
    if (commandCenter != nullptr) {
        commandCenter->getKeyMappings().keyPressed(key, this);
    }
    return true;
}

void MainComponent::mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& details) {
    if (commandCenter == nullptr) {
        return;
    }

    if (wheelCommandManager != nullptr) {
        wheelCommandManager->handleWheel(event, details);
    }
}
