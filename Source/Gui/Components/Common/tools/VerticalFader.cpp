#include "VerticalFader.h"

#include "Style/Font.h"

VerticalFader::VerticalFader()
    : faderDrawable(SvgFactory::load(SVG_Assets::FaderSvg)),
      buttonDrawable(SvgFactory::load(SVG_Assets::FaderButtonSvg)) {
}

void VerticalFader::paint(juce::Graphics& g) {
    if (faderDrawable == nullptr || buttonDrawable == nullptr) {
        return;
    }

    const auto faderBounds = getLocalBounds().toFloat().reduced(0.0f, 4.0f).withTrimmedBottom(20);
    const auto faderSvgBounds = faderDrawable->getDrawableBounds();
    const auto buttonSvgBounds = buttonDrawable->getDrawableBounds();
    if (faderSvgBounds.isEmpty() || buttonSvgBounds.isEmpty()) {
        return;
    }

    const float scale = faderBounds.getHeight() / faderSvgBounds.getHeight();
    const float faderOffsetX = faderBounds.getX() - faderSvgBounds.getX() * scale;
    const float faderOffsetY = faderBounds.getY() - faderSvgBounds.getY() * scale;
    const float lineX = faderBounds.getX() + (kLineX - faderSvgBounds.getX()) * scale;
    const float buttonWidth = buttonSvgBounds.getWidth() * scale;
    const float buttonHeight = buttonSvgBounds.getHeight() * scale;
    const float minY = faderBounds.getY() + kPadding;
    const float maxY = faderBounds.getBottom() - kPadding - buttonHeight;

    const auto clampedValue = std::clamp(value, 0.0f, 1.0f);
    const float buttonY = juce::jmap(clampedValue, 0.0f, 1.0f, maxY, minY);
    const float buttonX = lineX - buttonWidth * 0.5f;

    const auto faderTransform = juce::AffineTransform::scale(scale)
        .translated(faderOffsetX, faderOffsetY);
    faderDrawable->draw(g, 1.0f, faderTransform);

    const auto buttonTransform = juce::AffineTransform::scale(scale)
        .translated(buttonX - buttonSvgBounds.getX() * scale,
                    buttonY - buttonSvgBounds.getY() * scale);
    buttonDrawable->draw(g, 1.0f, buttonTransform);

    const auto textBounds = getLocalBounds().removeFromBottom(16);
    g.setColour(juce::Colour::fromString("#FF2F2C3F"));
    g.setFont(Fonts::p(13.0f, Fonts::Weight::Regular));
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

void VerticalFader::mouseDown(const juce::MouseEvent& event) {
    dragStartY = event.position.y;
    dragStartValue = value;
}

void VerticalFader::mouseDrag(const juce::MouseEvent& event) {
    const float deltaY = event.position.y - dragStartY;
    const auto bounds = getLocalBounds().toFloat().reduced(0.0f, 4.0f).withTrimmedBottom(20);
    const float usableHeight = std::max(1.0f, bounds.getHeight());
    const float deltaValue = (deltaY / usableHeight) * kDragSensitivity;
    setValue(dragStartValue - deltaValue);
}

float VerticalFader::getValue() const {
    return value;
}

float VerticalFader::getVolumeDb() const {
    const float clampedValue = std::clamp(value, 0.0f, 1.0f);
    const float zeroPosition = 1.0f - kZeroDbPosition;

    if (clampedValue >= zeroPosition) {
        const float t = (clampedValue - zeroPosition) / (1.0f - zeroPosition);
        return juce::jmap(t, 0.0f, 1.0f, 0.0f, kMaxVolumeDb);
    }

    if (clampedValue >= kMinus36DbPosition) {
        const float t = (clampedValue - kMinus36DbPosition) / (zeroPosition - kMinus36DbPosition);
        return juce::jmap(t, 0.0f, 1.0f, -36.0f, 0.0f);
    }

    const float t = clampedValue / kMinus36DbPosition;
    return juce::jmap(t, 0.0f, 1.0f, kMinVolumeDb, -36.0f);
}

void VerticalFader::setValue(float newValue) {
    value = std::clamp(newValue, 0.0f, 1.0f);
    repaint();
}

void VerticalFader::updateValueFromPosition(float y) {
    if (faderDrawable == nullptr || buttonDrawable == nullptr) {
        return;
    }
    const auto bounds = getLocalBounds().toFloat().reduced(0.0f, 4.0f).withTrimmedBottom(20);
    const auto faderSvgBounds = faderDrawable->getDrawableBounds();
    const auto buttonSvgBounds = buttonDrawable->getDrawableBounds();
    if (faderSvgBounds.isEmpty() || buttonSvgBounds.isEmpty()) {
        return;
    }
    const float scale = bounds.getHeight() / faderSvgBounds.getHeight();
    const float buttonHeight = buttonSvgBounds.getHeight() * scale;
    const float minY = bounds.getY() + kPadding;
    const float maxY = bounds.getBottom() - kPadding - buttonHeight;
    if (maxY <= minY) {
        return;
    }
    const float clampedY = std::clamp(y, minY, maxY);
    value = juce::jmap(clampedY, maxY, minY, 0.0f, 1.0f);
    repaint();
}
