#pragma once

#include <JuceHeader.h>
#include "Components/Components.h"
#include "Plugin/PluginEditorWindow.h"

#include <Utils/Transport.h>

#include "AudioEngine/AudioEngine.h"
#include "Components/Track/TrackContentPanel.h"
#include "Components/Track/TrackHeaderPanel.h"

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

private:
    std::unique_ptr<FileSelectorComponent> fileSelector;
    std::unique_ptr<RecordComponent> recordComponent;
    std::unique_ptr<PluginEditorWindow> pluginEditorWindow;
    std::shared_ptr<Edit> edit;
    std::unique_ptr<AudioEngine> audioEngine;
    bool isShutDown = false;
    juce::Slider volumeSlider;
    juce::AudioProcessorParameter* volumeParam = nullptr;
    juce::Slider stereoPanSlider;
    juce::AudioProcessorParameter* stereoPanParam = nullptr;
    juce::Slider reverbDrySlider;
    juce::TextButton zoomInButton{ "+" };
    juce::TextButton zoomOutButton{ "-" };
    juce::TextButton undoButton{ "Undo" };
    juce::TextButton redoButton{ "Redo" };

    std::unique_ptr<TrackHeaderPanel> trackHeaderPanel;
    std::unique_ptr<TrackContentPanel> trackContentPanel;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
