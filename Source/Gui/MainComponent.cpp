#include "MainComponent.h"

#include "AudioEngine/AudioEngine.h"
#include "Core/CustomEdits/ImportEdit.h"
#include "Utils/IO/EditSerializer.h"
#include "Utils/DebugWatchRegistry.h"
#include "Gui/Style/Font.h"
#include "Command/Commands.h"

namespace {
class WheelCommandForwarder final : public WheelForwardingViewport::Handler {
public:
    explicit WheelCommandForwarder(WheelCommandManager& wheelCommandManager)
        : wheelCommandManager(wheelCommandManager) {
    }

    bool handleWheel(const juce::MouseEvent& event,
                     const juce::MouseWheelDetails& details) override {
        return wheelCommandManager.handleWheel(event, details);
    }

private:
    WheelCommandManager& wheelCommandManager;
};
}

//==============================================================================
MainComponent::MainComponent()
{
    setSize (600, 400);

    edit = ImportEdit::importEdit();
    selectionManager = std::make_unique<SelectionManager>(*edit);
    cursorController = std::make_unique<CursorController>(*edit, *selectionManager);
    selectionManager->setCursorController(cursorController.get());
    commandCenter = std::make_unique<CommandCenter>(*edit, *cursorController);
    wheelCommandManager = std::make_unique<WheelCommandManager>(commandCenter->getCommandManager(),
                                                                commandCenter.get());
    wheelHandler = std::make_unique<WheelCommandForwarder>(*wheelCommandManager);
    trackViewport.setWheelHandler(wheelHandler.get());
    addKeyListener(&commandCenter->getKeyMappings());
    setWantsKeyboardFocus(true);
    grabKeyboardFocus();

    DebugWatchRegistry::get().setWatch("InsertionFollowsPlayback",
                                       &edit->getState(),
                                       [](const void* state) {
                                           const auto* editState = static_cast<const EditState*>(state);
                                           return juce::String(editState->getInsertionFollowsPlayback() ? "On" : "Off");
                                       });
    DebugWatchRegistry::get().setWatch("kHasSelectionRangeId",
                                       &edit->getState(),
                                       [](const void* state) {
                                           const auto* editState = static_cast<const EditState*>(state);
                                           return juce::String(editState->hasSelectionRange() ? "true" : "false");
                                       });
    DebugWatchRegistry::get().setWatch("kSelectionStartSampleId",
                                       &edit->getState(),
                                       [](const void* state) {
                                           const auto* editState = static_cast<const EditState*>(state);
                                           return juce::String(editState->getSelectionStartSample());
                                       });
    DebugWatchRegistry::get().setWatch("kSelectionEndSampleId",
                                       &edit->getState(),
                                       [](const void* state) {
                                           const auto* editState = static_cast<const EditState*>(state);
                                           return juce::String(editState->getSelectionEndSample());
                                       });
    DebugWatchRegistry::get().setWatch("kCursorSampleId",
                                       &edit->getState(),
                                       [](const void* state) {
                                           const auto* editState = static_cast<const EditState*>(state);
                                           return juce::String(editState->getCursorSample());
                                       });
    DebugWatchRegistry::get().setWatch("kIsLoopingId",
                                       &edit->getState(),
                                       [](const void* state) {
                                           const auto* editState = static_cast<const EditState*>(state);
                                           return juce::String(editState->getIsLooping() ? "true" : "false");
                                       });
    DebugWatchRegistry::get().setWatch("PlayheadSample",
                                       edit.get(),
                                       [](const void* editPtr) {
                                           const auto* editRef = static_cast<const Edit*>(editPtr);
                                           const auto transport = editRef->getTransport();
                                           if (transport == nullptr) {
                                               return juce::String("n/a");
                                           }
                                           return juce::String(transport->getPlayheadSample());
                                       });

    edit->getTransport()->setFrameRate(24);

    const auto baseFont = Fonts::p(14.0f, Fonts::Weight::Regular);
    juce::LookAndFeel::getDefaultLookAndFeel().setDefaultSansSerifTypeface(baseFont.getTypefacePtr());
    audioEngine = std::make_unique<AudioEngine>(edit);

    header = std::make_unique<Header>(*edit, commandCenter->getCommandManager());
    addAndMakeVisible(header.get());

    contextualSection = std::make_unique<ContextualSection>();
    addAndMakeVisible(contextualSection.get());

    trackHeaderPanel = std::make_unique<TrackHeaderPanel>(*edit, *selectionManager);
    trackAreaContainer.addAndMakeVisible(trackHeaderPanel.get());

    trackContentPanel = std::make_unique<TrackContentPanel>(*edit, *selectionManager);
    trackAreaContainer.addAndMakeVisible(trackContentPanel.get());

    trackHorizontalScrollBar = std::make_unique<TrackHorizontalScrollBar>(*edit);
    addAndMakeVisible(trackHorizontalScrollBar.get());

    trackViewport.setViewedComponent(&trackAreaContainer, false);
    trackViewport.setScrollBarsShown(true, false);
    trackViewport.setLookAndFeel(&trackScrollbarLookAndFeel);
    addAndMakeVisible(trackViewport);

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
    trackViewport.setLookAndFeel(nullptr);
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
    if (trackViewport.isShowing()) {
        const int headerWidth = 200;
        const int scrollBarHeight = trackHorizontalScrollBar != nullptr
            ? trackHorizontalScrollBar->getPreferredHeight()
            : 0;
        auto trackAreaBounds = bounds;
        trackViewport.setBounds(trackAreaBounds);
        const int contentWidth = std::max(0, trackViewport.getWidth() - headerWidth);
        int totalHeight = edit != nullptr ? edit->getState().getTimelineHeight() : 0;
        if (edit != nullptr) {
            for (const auto& track : edit->getTracks()) {
                if (!track) {
                    continue;
                }
                totalHeight += std::max(1, static_cast<int>(track->getHeight()));
            }
        }
        totalHeight += scrollBarHeight;
        trackAreaContainer.setSize(trackViewport.getWidth(), totalHeight);
        if (trackHeaderPanel != nullptr) {
            trackHeaderPanel->setBounds(0, 0, headerWidth, totalHeight);
        }
        if (trackContentPanel != nullptr) {
            trackContentPanel->setBounds(headerWidth, 0, contentWidth, totalHeight);
        }
        if (trackHorizontalScrollBar != nullptr) {
            auto scrollBarArea = trackAreaBounds.removeFromBottom(scrollBarHeight);
            scrollBarArea.removeFromLeft(headerWidth);
            const int scrollBarMaxWidth = std::max(0, contentWidth - 8);
            scrollBarArea.setWidth(std::min(scrollBarArea.getWidth(), scrollBarMaxWidth));
            trackHorizontalScrollBar->setBounds(scrollBarArea);
            trackHorizontalScrollBar->toFront(false);
        }
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
