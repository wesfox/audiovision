#include "CursorTimeline.h"

CursorTimeline::CursorTimeline(const Edit& edit, int rulerHeight) : edit(edit), rulerHeight(rulerHeight) {
    cursorDrawable = SvgFactory::load(SVG_Assets::TimelineCursorSvg, fillColour);
}

namespace {
int64 positionToSample(const Edit& edit, float x, float width) {
    const auto viewStart = edit.getViewStartSample();
    const auto viewEnd = edit.getViewEndSample();
    const auto viewLength = viewEnd - viewStart;
    if (viewLength <= 0 || width <= 0.0f) {
        return viewStart;
    }
    const double proportion = std::clamp(static_cast<double>(x / width), 0.0, 1.0);
    return viewStart + static_cast<int64>(std::llround(proportion * static_cast<double>(viewLength)));
}
}

void CursorTimeline::paint(juce::Graphics& g) {
    if (cursorDrawable == nullptr) {
        return;
    }

    const auto viewStart = edit.getViewStartSample();
    const auto viewEnd = edit.getViewEndSample();
    const auto viewLength = viewEnd - viewStart;
    if (viewLength <= 0) {
        return;
    }

    const auto transport = edit.getTransport();
    if (!transport) {
        return;
    }

    const auto cursorSample = transport->getCursorPosition();
    const double position = (static_cast<double>(cursorSample - viewStart) /
                             static_cast<double>(viewLength));
    if (position < 0.0 || position > 1.0) {
        return;
    }

    const auto bounds = getLocalBounds();
    const float x = static_cast<float>(position) * static_cast<float>(bounds.getWidth());
    const float alignedX = std::floor(x) + 0.5f;

    const auto drawableBounds = cursorDrawable->getDrawableBounds();
    const float targetHeight = static_cast<float>(rulerHeight) * (2.0f / 3.0f);
    const float scale = drawableBounds.getHeight() > 0.0f
        ? (targetHeight / drawableBounds.getHeight())
        : 1.0f;
    const float scaledWidth = drawableBounds.getWidth() * scale;
    const float drawX = (x - scaledWidth * 0.5f);
    const float drawY = static_cast<float>(rulerHeight) - (drawableBounds.getHeight() * scale);

    g.setColour(juce::Colour::fromRGBA(76, 44, 126, 120));
    g.drawLine(alignedX, static_cast<float>(rulerHeight), alignedX, static_cast<float>(bounds.getHeight()), 1.0f);

    juce::AffineTransform transform = juce::AffineTransform::scale(scale)
        .translated(drawX, drawY);
    cursorDrawable->draw(g, 1.0f, transform);
}

bool CursorTimeline::hitTest(int x, int y) {
    return y >= 0 && y <= rulerHeight;
}

void CursorTimeline::mouseDown(const juce::MouseEvent& event) {
    isDragging = false;
    const auto newSample = positionToSample(edit, event.position.x, static_cast<float>(getWidth()));
    if (auto transport = edit.getTransport()) {
        pointerDownSample = transport->getCursorPosition();
    } else {
        pointerDownSample = newSample;
    }
    if (callbacks.onPointerDown) {
        callbacks.onPointerDown(pointerDownSample, newSample);
    }
    repaint();
}

void CursorTimeline::mouseDrag(const juce::MouseEvent& event) {
    isDragging = true;
    const auto newSample = positionToSample(edit, event.position.x, static_cast<float>(getWidth()));
    if (callbacks.onPointerDrag) {
        callbacks.onPointerDrag(newSample);
    }
    repaint();
}

void CursorTimeline::mouseUp(const juce::MouseEvent& event) {
    const auto newSample = positionToSample(edit, event.position.x, static_cast<float>(getWidth()));
    if (callbacks.onPointerUp) {
        callbacks.onPointerUp(pointerDownSample, newSample, isDragging);
    }
    isDragging = false;
    repaint();
}

void CursorTimeline::setCallbacks(Callbacks newCallbacks) {
    callbacks = std::move(newCallbacks);
}

void CursorTimeline::setColour(juce::Colour colour) {
    fillColour = colour;
    cursorDrawable = SvgFactory::load(SVG_Assets::TimelineCursorSvg, fillColour);
    repaint();
}

void CursorTimeline::setRulerHeight(int height) {
    rulerHeight = height;
}
