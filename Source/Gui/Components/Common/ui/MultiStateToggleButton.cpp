#include "MultiStateToggleButton.h"

#include <algorithm>

MultiStateToggleButton::MultiStateToggleButton(const String& textContent)
    : textContent(textContent) {
}

void MultiStateToggleButton::setState(StateId newState, bool notify) {
    applyState(newState, notify);
}

MultiStateToggleButton::StateId MultiStateToggleButton::getState() const {
    return state;
}

void MultiStateToggleButton::setColors(StateId stateId, juce::Colour backgroundColor, juce::Colour textColor) {
    coloursByState[stateId] = { backgroundColor, textColor };
    repaint();
}

void MultiStateToggleButton::setStateCycle(std::vector<StateId> cycle) {
    if (cycle.empty()) {
        // State cycle must contain at least one state.
        jassert(false);
        return;
    }
    stateCycle = std::move(cycle);
}

void MultiStateToggleButton::onNewState(std::function<void(StateId)> callback) {
    onNewStateCallback = std::move(callback);
}

void MultiStateToggleButton::resized() {
}

void MultiStateToggleButton::paint(juce::Graphics& g) {
    auto b = getLocalBounds();
    g.setFont(14.0f);
    const auto colourIt = coloursByState.find(state);
    if (colourIt == coloursByState.end()) {
        // State colors must be configured before painting.
        jassert(false);
        return;
    }
    constexpr float cornerRadius = 10.0f;
    g.setColour(colourIt->second.background);
    g.fillRoundedRectangle(b.toFloat(), cornerRadius);
    g.setColour(colourIt->second.background.darker(0.2f));
    const auto inset = b.toFloat().reduced(0.5f);
    g.drawRoundedRectangle(inset, cornerRadius - 0.5f, 1.0f);
    g.setColour(colourIt->second.text);
    g.drawText(textContent, b, Justification::centred);
}

void MultiStateToggleButton::mouseDown(const juce::MouseEvent& e) {
    juce::ignoreUnused(e);
    applyState(getNextState(), true);
}

MultiStateToggleButton::StateId MultiStateToggleButton::getNextState() const {
    if (stateCycle.empty()) {
        // State cycle must contain at least one state.
        jassert(false);
        return state;
    }
    auto it = std::find(stateCycle.begin(), stateCycle.end(), state);
    if (it == stateCycle.end()) {
        return stateCycle.front();
    }
    ++it;
    if (it == stateCycle.end()) {
        return stateCycle.front();
    }
    return *it;
}

void MultiStateToggleButton::applyState(StateId nextState, bool notify) {
    if (state == nextState) {
        return;
    }
    state = nextState;
    repaint();
    if (notify && onNewStateCallback) {
        onNewStateCallback(state);
    }
}
