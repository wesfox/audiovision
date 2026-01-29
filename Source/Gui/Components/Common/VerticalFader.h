#pragma once

#include <JuceHeader.h>

#include "Gui/Assets/SvgFactory.h"

/// Displays a draggable fader with SVG assets.
class VerticalFader : public juce::Component {
public:
    VerticalFader();

    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;

    float getValue() const;
    void setValue(float newValue);
    float getVolumeDb() const;

    static constexpr float kLineX = 14.5f;
    static constexpr float kPadding = -2.0f;
    static constexpr float kMinVolumeDb = -60.0f;
    static constexpr float kMaxVolumeDb = 12.0f;
    static constexpr float kZeroDbPosition = 2.0f / 9.0f;
    static constexpr float kDragSensitivity = 0.3f;

private:
    void updateValueFromPosition(float y);

    std::unique_ptr<juce::Drawable> faderDrawable;
    std::unique_ptr<juce::Drawable> buttonDrawable;
    float value = 0.5f;
    float dragStartValue = 0.0f;
    float dragStartY = 0.0f;
};
