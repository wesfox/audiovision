#include "EditableText.h"

// ------------------------ MainComponent Implementation ------------------------

EditableText::EditableText(const String& textContent, const juce::Colour backgroundColour, const juce::Colour textColour) :
    backgroundColour(backgroundColour),
    textColour(textColour) {
    textEditor.setColour(juce::TextEditor::ColourIds::textColourId,textColour);
    textEditor.setColour(juce::TextEditor::ColourIds::highlightedTextColourId,textColour);
    textEditor.setColour(juce::TextEditor::ColourIds::backgroundColourId,backgroundColour);
    textEditor.setJustification(Justification::centred);
    textEditor.setColour(juce::TextEditor::outlineColourId, backgroundColour);
    textEditor.setColour(juce::TextEditor::focusedOutlineColourId, backgroundColour);
    textEditor.setColour(juce::TextEditor::ColourIds::highlightColourId,juce::Colour::fromRGBA(0,0,0,20));
    
    // reset all border / padding
    textEditor.setIndents(0, 0);
    
    textEditor.setText(textContent);

    addAndMakeVisible(textEditor);

    textEditor.addMouseListener(this, false);
    textEditor.setWantsKeyboardFocus(false);
}

void EditableText::resized() {
    textEditor.setBounds(getLocalBounds().reduced(1));
}

void EditableText::paint(juce::Graphics &g) {
}

void EditableText::mouseDown(const juce::MouseEvent &e) {
    if (e.eventComponent == &textEditor) {
        textEditor.setWantsKeyboardFocus(true);
    }
}

void EditableText::setContent(const juce::String& content) {
    textEditor.setText(content);
}
