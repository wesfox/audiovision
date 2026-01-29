#include "RotaryFader.h"

RotaryFader::RotaryFader()
    : knobDrawable(SvgFactory::load(SVG_Assets::RotaryFaderSvg)) {
}

void RotaryFader::paint(juce::Graphics& g) {
    if (knobDrawable == nullptr) {
        return;
    }

    const auto bounds = getLocalBounds().toFloat().withTrimmedTop(16);
    const auto svgBounds = knobDrawable->getDrawableBounds();
    if (svgBounds.isEmpty()) {
        return;
    }

    const float scale = std::min(bounds.getWidth() / svgBounds.getWidth(),
                                 bounds.getHeight() / svgBounds.getHeight());
    const float angle = juce::jmap(std::clamp(value, 0.0f, 1.0f),
                                   0.0f,
                                   1.0f,
                                   kMinAngle,
                                   kMaxAngle);

    juce::AffineTransform transform = juce::AffineTransform::translation(-svgBounds.getCentreX(),
                                                                          -svgBounds.getCentreY())
        .scaled(scale)
        .rotated(angle)
        .translated(bounds.getCentreX(), bounds.getCentreY());

    knobDrawable->draw(g, 1.0f, transform);

    auto textBounds = getLocalBounds().removeFromTop(14);
    g.setColour(juce::Colour::fromString("#FF2F2C3F"));
    g.setFont(juce::Font(13.0f));
    auto volume = getVolumeDb();
    auto serializedVolume = juce::String(getVolumeDb(), 1);
    if (volume < kMinVolumeDb + 0.00001f) {
        serializedVolume = juce::String::fromUTF8(" ∞ ");
    }
    g.drawText(serializedVolume + " dB",
               textBounds,
               juce::Justification::topLeft,
               false);

}

void RotaryFader::mouseDown(const juce::MouseEvent& event) {
    dragStartY = event.position.y;
    dragStartValue = value;
}

void RotaryFader::mouseDrag(const juce::MouseEvent& event) {
    const float deltaY = event.position.y - dragStartY;
    const float usableHeight = std::max(1.0f, static_cast<float>(getHeight()));
    const float deltaValue = (deltaY / usableHeight) * kDragSensitivity;
    setValue(dragStartValue - deltaValue);
}

float RotaryFader::getValue() const {
    return value;
}

float RotaryFader::getVolumeDb() const {
    const float clampedValue = std::clamp(value, 0.0f, 1.0f);
    if (clampedValue >= 1.0f - kZeroDbPosition) {
        const float t = (clampedValue - (1.0f - kZeroDbPosition)) / kZeroDbPosition;
        return juce::jmap(t, 0.0f, 1.0f, 0.0f, kMaxVolumeDb);
    }
    const float t = clampedValue / (1.0f - kZeroDbPosition);
    return juce::jmap(t, 0.0f, 1.0f, kMinVolumeDb, 0.0f);
}

void RotaryFader::setValue(float newValue) {
    value = std::clamp(newValue, 0.0f, 1.0f);
    repaint();
}
