#include "CursorTimeline.h"

#include "Gui/Utils/ViewRangeMapper.h"

CursorTimeline::CursorTimeline(const Edit& edit, int rulerHeight) : edit(edit), rulerHeight(rulerHeight) {
    cursorDrawable = SvgFactory::load(SVG_Assets::TimelineCursorSvg, fillColour);
}

void CursorTimeline::paint(juce::Graphics& g) {
    if (cursorDrawable == nullptr) {
        return;
    }

    const auto mapper = getMapper();

    const auto transport = edit.getTransport();
    if (!transport) {
        return;
    }

    const auto playheadSample = transport->getPlayheadSample();
    if (playheadSample < mapper.getViewStartSample() || playheadSample > mapper.getViewEndSample()) {
        return;
    }

    const auto bounds = getLocalBounds();
    const float x = mapper.sampleToX(playheadSample);
    const float alignedX = std::floor(x) + 0.5f;

    const auto drawableBounds = cursorDrawable->getDrawableBounds();
    const float targetHeight = static_cast<float>(rulerHeight) * (2.0f / 3.0f);
    const float scale = drawableBounds.getHeight() > 0.0f
        ? (targetHeight / drawableBounds.getHeight())
        : 1.0f;
    const float scaledWidth = drawableBounds.getWidth() * scale;
    const float drawX = (x - scaledWidth * 0.5f);
    const float drawY = static_cast<float>(rulerHeight) - (drawableBounds.getHeight() * scale);

    if (transport->isPlaying()){
        g.setColour(juce::Colour::fromRGBA(76, 44, 126, 120));
        g.drawLine(alignedX, static_cast<float>(rulerHeight), alignedX, static_cast<float>(bounds.getHeight()), 1.0f);
    }

    juce::AffineTransform transform = juce::AffineTransform::scale(scale)
        .translated(drawX, drawY);
    cursorDrawable->draw(g, 1.0f, transform);
}

bool CursorTimeline::hitTest(int x, int y) {
    return y >= 0 && y <= rulerHeight;
}

void CursorTimeline::mouseDown(const juce::MouseEvent& event) {
    isDragging = false;
    const auto mapper = getMapper();
    const auto newSample = mapper.xToSample(event.position.x);
    if (auto transport = edit.getTransport()) {
        pointerDownSample = transport->getPlayheadSample();
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
    const auto mapper = getMapper();
    const auto newSample = mapper.xToSample(event.position.x);
    if (callbacks.onPointerDrag) {
        callbacks.onPointerDrag(newSample);
    }
    repaint();
}

void CursorTimeline::mouseUp(const juce::MouseEvent& event) {
    const auto mapper = getMapper();
    const auto newSample = mapper.xToSample(event.position.x);
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

ViewRangeMapper CursorTimeline::getMapper() const {
    ViewRangeMapper mapper(edit, static_cast<float>(getWidth()));
    if (!mapper.isValid()) {
        // View range and width must be valid to map samples.
        jassert(false);
    }
    return mapper;
}
