#pragma once

#include <JuceHeader.h>
#include "Components/Components.h"
#include "Plugin/PluginEditorWindow.h"

#include <Utils/Transport.h>

#include "AudioEngine/AudioEngine.h"

class EditTest;

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::Component
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;
    void shutdown();

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    //==============================================================================
    // Your private member variables go here...
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


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
