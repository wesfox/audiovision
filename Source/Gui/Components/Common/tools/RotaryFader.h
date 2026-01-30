#pragma once

#include <JuceHeader.h>

#include "Gui/Assets/SvgFactory.h"

class RotaryFader : public juce::Component {
public:
    RotaryFader();

    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;

    float getValue() const;
    void setValue(float newValue);
    float getVolumeDb() const;
    void setLabel(std::optional<juce::String> newLabel);

    static constexpr float kDragSensitivity = 0.1f;
    static constexpr float kMinAngle = -juce::MathConstants<float>::pi * 0.75f;
    static constexpr float kMaxAngle = juce::MathConstants<float>::pi * 0.75f;
    static constexpr float kZeroPosition = 0.5f;
    static constexpr float kMinVolumeDb = -60.0f;
    static constexpr float kMaxVolumeDb = 12.0f;
    static constexpr float kZeroDbPosition = 2.0f / 9.0f;

private:
    std::unique_ptr<juce::Drawable> knobDrawable;
    std::optional<juce::String> label;
    float value = kZeroPosition;
    float dragStartValue = 0.0f;
    float dragStartY = 0.0f;
};
