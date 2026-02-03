#pragma once

#include <JuceHeader.h>

#include <functional>
#include <map>
#include <vector>

/// Toggle button with multiple visual states.
class MultiStateToggleButton : public juce::Component {
public:
    using StateId = int;

    /// Create a toggle button with fixed label text.
    /// @param textContent label to display
    explicit MultiStateToggleButton(const String& textContent);

    /// Set the current state.
    /// @param state new state value
    /// @param notify true to trigger the callback
    void setState(StateId state, bool notify = true);

    /// Get the current state.
    StateId getState() const;

    /// Set colors for a state.
    /// @param state state identifier
    /// @param backgroundColor background fill color
    /// @param textColor text color
    void setColors(StateId state, juce::Colour backgroundColor, juce::Colour textColor);

    /// Set the cycle order for click transitions.
    /// @param cycle ordered list of state identifiers
    void setStateCycle(std::vector<StateId> cycle);

    /// Register a callback for state changes.
    /// @param callback function called after state changes
    void onNewState(std::function<void(StateId)> callback);

    void resized() override;
    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& e) override;

private:
    struct StateColours {
        juce::Colour background;
        juce::Colour text;
    };

    StateId getNextState() const;
    void applyState(StateId nextState, bool notify);

    String textContent;
    StateId state = 0;
    std::map<StateId, StateColours> coloursByState;
    std::vector<StateId> stateCycle { 0, 1 };
    std::function<void(StateId)> onNewStateCallback;
};
