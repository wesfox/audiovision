#pragma once

#include <JuceHeader.h>

#include <functional>

/// Displays a pill-style button with three visual states.
class RoundedButton : public juce::Component {
public:
    enum class State {
        Active,
        Clicked,
        Inactive
    };

    explicit RoundedButton(juce::String text, State initialState = State::Active);

    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;

    void setState(State newState);
    State getState() const;

    void setLabel(juce::String text);

    std::function<void()> onClick;

private:
    juce::String label;
    State state = State::Active;
    State baseState = State::Active;
};
