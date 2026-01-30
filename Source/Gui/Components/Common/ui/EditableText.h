#pragma once

#include <JuceHeader.h>

class EditableText : public juce::Component {
public:
    EditableText(const String& textContent, juce::Colour backgroundColour, juce::Colour textColour);

    void resized() override;

    void paint(juce::Graphics& g) override;

    void mouseDown (const juce::MouseEvent& e) override;

    void setContent(const juce::String& content);

private:
    juce::Colour backgroundColour;
    juce::Colour textColour;
    juce::TextEditor textEditor;
};
