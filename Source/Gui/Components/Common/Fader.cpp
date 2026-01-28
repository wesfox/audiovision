#include "Fader.h"

Fader::Fader()
    : faderDrawable(SvgFactory::load(SVG_Assets::FaderSvg)),
      buttonDrawable(SvgFactory::load(SVG_Assets::FaderButtonSvg)) {
}

void Fader::paint(juce::Graphics& g) {
    if (faderDrawable == nullptr || buttonDrawable == nullptr) {
        return;
    }

    const auto bounds = getLocalBounds().toFloat().withTrimmedTop(12).withTrimmedBottom(20);
    const auto buttonBounds = buttonDrawable->getDrawableBounds();
    if (buttonBounds.isEmpty()) {
        return;
    }

    const auto metrics = getMetrics();

    juce::AffineTransform trackTransform = juce::AffineTransform::scale(metrics.scale)
        .scaled(0.84, 0.84).translated(bounds.getX(), bounds.getY());
    faderDrawable->draw(g, 1.0f, trackTransform);

    const float clampedValue = std::clamp(value, 0.0f, 1.0f);
    const float buttonY = juce::jmap(clampedValue, 0.0f, 1.0f, metrics.maxY, metrics.minY);
    const float buttonX = metrics.lineX - metrics.buttonWidth * 0.5f;

    juce::AffineTransform buttonTransform = juce::AffineTransform::scale(metrics.scale)
        .translated(buttonX-18, buttonY-45);
    buttonDrawable->draw(g, 1.0f, buttonTransform);

    const auto textBounds = getLocalBounds().removeFromBottom(16);
    g.setColour(juce::Colour::fromString("#FF2F2C3F"));
    g.setFont(juce::Font(11.0f));
    g.drawText(juce::String(getVolumeDb(), 1) + " dB",
               textBounds,
               juce::Justification::centredTop,
               false);
    g.fillRect(faderDrawable->getDrawableBounds());
}

void Fader::mouseDown(const juce::MouseEvent& event) {
    updateValueFromPosition(event.position.y);
}

void Fader::mouseDrag(const juce::MouseEvent& event) {
    updateValueFromPosition(event.position.y);
}

float Fader::getValue() const {
    return value;
}

float Fader::getVolumeDb() const {
    const float clampedValue = std::clamp(value, 0.0f, 1.0f);
    if (clampedValue >= 1.0f - kZeroDbPosition) {
        const float t = (clampedValue - (1.0f - kZeroDbPosition)) / kZeroDbPosition;
        return juce::jmap(t, 0.0f, 1.0f, 0.0f, kMaxVolumeDb);
    }
    const float t = clampedValue / (1.0f - kZeroDbPosition);
    return juce::jmap(t, 0.0f, 1.0f, kMinVolumeDb, 0.0f);
}

void Fader::setValue(float newValue) {
    value = std::clamp(newValue, 0.0f, 1.0f);
    repaint();
}

Fader::LayoutMetrics Fader::getMetrics() const {
    LayoutMetrics metrics;
    const auto bounds = getLocalBounds().toFloat().reduced(0.0f, 4.0f);
    if (faderDrawable == nullptr || buttonDrawable == nullptr) {
        return metrics;
    }

    const auto faderBounds = faderDrawable->getDrawableBounds();
    const auto buttonBounds = buttonDrawable->getDrawableBounds();
    if (faderBounds.isEmpty() || buttonBounds.isEmpty()) {
        return metrics;
    }

    metrics.scale = bounds.getHeight() / faderBounds.getHeight();
    metrics.lineX = bounds.getX() + kLineX * metrics.scale;
    metrics.buttonWidth = buttonBounds.getWidth() * metrics.scale;
    metrics.buttonHeight = buttonBounds.getHeight() * metrics.scale;
    metrics.minY = bounds.getY() + kPadding;
    metrics.maxY = bounds.getBottom() - kPadding - metrics.buttonHeight;
    return metrics;
}

void Fader::updateValueFromPosition(float y) {
    const auto metrics = getMetrics();
    if (metrics.maxY <= metrics.minY) {
        return;
    }
    const float clampedY = std::clamp(y, metrics.minY, metrics.maxY);
    value = juce::jmap(clampedY, metrics.maxY, metrics.minY, 0.0f, 1.0f);
    repaint();
}
