#pragma once

#include <JuceHeader.h>
#include "Components/Header/Header.h"
#include "Components/ContextualSection/ContextualSection.h"
#include "Command/CommandCenter.h"
#include "Command/WheelCommandManager.h"
#include "Command/TrackCommandManager.h"
#include "Gui/Utils/CursorController.h"
#include "Gui/Utils/SelectionManager.h"
#include "Gui/Utils/DebugWatchWindow.h"
#include "Gui/Utils/WheelForwardingViewport.h"
#include "Gui/Video/VideoView.h"

#include "AudioEngine/AudioEngine.h"
#include "Components/Track/TrackContentPanel.h"
#include "Components/Track/TrackHeaderPanel.h"
#include "Components/Track/TrackHorizontalScrollBar.h"
#include "Gui/Style/LookAndFeel/ScrollBarLook.h"

class EditTest;

/// Main application UI root component.
class MainComponent  : public juce::Component
{
public:
    /// Create the main UI and engine.
    MainComponent();
    ~MainComponent() override;

    /// Release resources and stop audio.
    void shutdown();

    /// Draw the background.
    void paint (juce::Graphics&) override;

    /// Layout child components.
    void resized() override;
    bool keyPressed(const juce::KeyPress& key) override;
    void mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& details) override;

private:
    std::shared_ptr<Edit> edit;
    std::unique_ptr<AudioEngine> audioEngine;
    bool isShutDown = false;
    std::unique_ptr<CommandCenter> commandCenter;
    std::unique_ptr<WheelCommandManager> wheelCommandManager;
    std::unique_ptr<TrackCommandManager> trackCommandManager;
    std::unique_ptr<CursorController> cursorController;
    std::unique_ptr<SelectionManager> selectionManager;

    std::unique_ptr<TrackHeaderPanel> trackHeaderPanel;
    std::unique_ptr<TrackContentPanel> trackContentPanel;
    std::unique_ptr<TrackHorizontalScrollBar> trackHorizontalScrollBar;
    std::unique_ptr<WheelForwardingViewport::Handler> wheelHandler;
    WheelForwardingViewport trackViewport;
    juce::Component trackAreaContainer;
    ScrollBarLook trackScrollbarLookAndFeel;
    std::unique_ptr<Header> header;
    std::unique_ptr<ContextualSection> contextualSection;
    std::unique_ptr<VideoView> videoView;
    std::unique_ptr<juce::DocumentWindow> videoWindow;
    std::unique_ptr<DebugWatchWindow> debugWatchWindow;
    juce::File lastExportDirectory;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
