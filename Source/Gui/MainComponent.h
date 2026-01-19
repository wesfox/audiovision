#pragma once

#include <JuceHeader.h>
#include "Components/Components.h"

#include <Utils/Transport.h>

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

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    //==============================================================================
    // Your private member variables go here...
    std::unique_ptr<FileSelectorComponent> fileSelector;
    std::unique_ptr<AudioOutputComponent> audioOutputComponent;
    std::unique_ptr<EditTest> edit;
    Transport transport;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
