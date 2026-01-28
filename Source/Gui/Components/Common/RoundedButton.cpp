#include "RoundedButton.h"

#include "Gui/Style/Font.h"

RoundedButton::RoundedButton(juce::String text, State initialState)
    : label(std::move(text)),
      state(initialState),
      baseState(initialState) {
}

void RoundedButton::paint(juce::Graphics& g) {
    const auto bounds = getLocalBounds().toFloat().reduced(0.5f);
    const auto radius = 8.0f;
    const auto activeFill = juce::Colour::fromString("#FFF4F1FF");
    const auto activeStroke = juce::Colour::fromString("#FFC9BFFF");
    const auto clickedFill = juce::Colour::fromString("#FFE8E1FF");
    const auto clickedStroke = juce::Colour::fromString("#FFB4A6FF");
    const auto inactiveFill = juce::Colour::fromString("#FFF7F5FF");
    const auto inactiveStroke = juce::Colour::fromString("#FFDAD3FF");

    juce::Colour fillColour = activeFill;
    juce::Colour strokeColour = activeStroke;
    switch (state) {
        case State::Clicked:
            fillColour = clickedFill;
            strokeColour = clickedStroke;
            break;
        case State::Inactive:
            fillColour = inactiveFill;
            strokeColour = inactiveStroke;
            break;
        case State::Active:
            fillColour = activeFill;
            strokeColour = activeStroke;
        default:
            break;
    }

    g.setColour(fillColour);
    g.fillRoundedRectangle(bounds, radius);

    g.setColour(strokeColour);
    g.drawRoundedRectangle(bounds, radius, 1.0f);

    auto textColour = juce::Colours::black.withAlpha(state == State::Inactive ? 0.5f : 0.9f);
    g.setColour(textColour);
    g.setFont(Fonts::p(12.0f, Fonts::Weight::Regular));
    g.drawText(label, getLocalBounds(), juce::Justification::centred, false);
}

void RoundedButton::mouseDown(const juce::MouseEvent& event) {
    juce::Component::mouseDown(event);
    // if (baseState == State::Inactive) {
    //     return;
    // }
    state = State::Clicked;
    repaint();
}

void RoundedButton::mouseUp(const juce::MouseEvent& event) {
    juce::Component::mouseUp(event);
    state = baseState;
    repaint();
    if (onClick) {
        onClick();
    }
}

void RoundedButton::setState(State newState) {
    baseState = newState;
    if (state != State::Clicked) {
        state = newState;
    }
    repaint();
}

RoundedButton::State RoundedButton::getState() const {
    return baseState;
}

void RoundedButton::setLabel(juce::String text) {
    label = std::move(text);
    repaint();
}
