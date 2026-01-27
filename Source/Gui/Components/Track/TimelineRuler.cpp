#include "TimelineRuler.h"

namespace {
constexpr int kRulerHeight = 20;
constexpr float kMinTickSpacingPx = 3.0f;
constexpr float kMinLabelSpacingPx = 60.0f;

juce::String formatTimecode(double seconds, double frameRate) {
    if (seconds < 0.0) {
        seconds = 0.0;
    }
    const int totalSeconds = static_cast<int>(std::floor(seconds));
    const int hours = totalSeconds / 3600;
    const int minutes = (totalSeconds / 60) % 60;
    const int secs = totalSeconds % 60;
    const double fractional = seconds - static_cast<double>(totalSeconds);
    const int frames = static_cast<int>(std::floor(fractional * frameRate + 1.0e-6));

    return juce::String::formatted("%02d:%02d:%02d:%02d", hours, minutes, secs, frames);
}

double snapToFrame(double seconds, double frameRate) {
    if (frameRate <= 0.0) {
        return seconds;
    }
    const double frames = std::round(seconds * frameRate);
    return frames / frameRate;
}

struct TickRate {
    double secondsPerTick;
};

TickRate pickMinorTickRate(double frameRate, double pixelsPerSecond) {
    const double frameSeconds = frameRate > 0.0 ? 1.0 / frameRate : 0.0;
    const TickRate candidates[] = {
        { frameSeconds },
        { frameSeconds * 2 },
        { 1.0 },
        { 10.0 },
        { 60.0 },
        { 600.0 },
        { 3600.0 }
    };

    TickRate selected = candidates[0];
    for (const auto& candidate : candidates) {
        if (candidate.secondsPerTick <= 0.0) {
            continue;
        }
        const double spacing = candidate.secondsPerTick * pixelsPerSecond;
        if (spacing >= kMinTickSpacingPx) {
            selected = candidate;
            break;
        }
        selected = candidate;
    }
    return selected;
}

TickRate pickMajorTickRate(const TickRate& minor) {
    const double minorSeconds = minor.secondsPerTick;
    if (minorSeconds <= 0.0) {
        return minor;
    }
    if (minorSeconds < 1.0) return { 1.0 };
    if (minorSeconds < 10.0) return { 10.0 };
    if (minorSeconds < 60.0) return { 60.0 };
    if (minorSeconds < 600.0) return { 600.0 };
    if (minorSeconds < 3600.0) return { 3600.0 };
    return minor;
}

int64 getMajorStepFrames(double frameRate, const TickRate& minor, const TickRate& major) {
    if (frameRate <= 0.0) {
        return 1;
    }
    const int64 minorFrames = std::max<int64>(1, static_cast<int64>(std::llround(minor.secondsPerTick * frameRate)));
    const int64 majorFrames = std::max<int64>(minorFrames,
        static_cast<int64>(std::llround(major.secondsPerTick * frameRate)));
    return std::max<int64>(1, majorFrames / minorFrames);
}
}

TimelineRuler::TimelineRuler(const Edit& edit) : edit(edit) {
    setSize(0, kRulerHeight);
}

void TimelineRuler::paint(juce::Graphics& g) {
    auto bounds = getLocalBounds();
    g.setColour(juce::Colour::fromRGB(245, 245, 245));
    g.fillRect(bounds);

    const auto viewStart = edit.getViewStartSample();
    const auto viewEnd = edit.getViewEndSample();
    const auto viewLength = viewEnd - viewStart;
    if (viewLength <= 0) {
        return;
    }

    const double sampleRate = 48000.0;
    const double frameRate = static_cast<double>(edit.getFrameRate());
    const double viewSeconds = static_cast<double>(viewLength) / sampleRate;
    const float width = static_cast<float>(bounds.getWidth());

    if (viewSeconds <= 0.0 || frameRate <= 0.0) {
        return;
    }

    const double pixelsPerSecond = width / viewSeconds;
    const auto minorTick = pickMinorTickRate(frameRate, pixelsPerSecond);
    const auto majorTick = pickMajorTickRate(minorTick);
    const int64 majorStepFrames = getMajorStepFrames(frameRate, minorTick, majorTick);

    const double viewStartSeconds = static_cast<double>(viewStart) / sampleRate;
    const double viewEndSeconds = static_cast<double>(viewEnd) / sampleRate;
    const double minorStart = std::floor(viewStartSeconds / minorTick.secondsPerTick) * minorTick.secondsPerTick;
    const double minorEnd = std::ceil(viewEndSeconds / minorTick.secondsPerTick) * minorTick.secondsPerTick;

    g.setFont(10.0f);
    int64 tickIndex = 0;
    float lastLabelX = -kMinLabelSpacingPx;
    for (double t = minorStart; t <= minorEnd; t += minorTick.secondsPerTick, ++tickIndex) {
        const double snappedTime = snapToFrame(t, frameRate);
        const double sampleAtTime = snappedTime * sampleRate;
        const float x = static_cast<float>((sampleAtTime - static_cast<double>(viewStart)) /
                                           static_cast<double>(viewLength)) * width;
        const float alignedX = std::floor(x) + 0.5f;
        if (alignedX < 0.0f || alignedX > width) {
            continue;
        }
        if (tickIndex % majorStepFrames == 0) {
            const bool canShowLabel = (alignedX - lastLabelX) >= kMinLabelSpacingPx;
            if (canShowLabel) {
                g.setColour(juce::Colour::fromRGB(120, 120, 120));
                g.drawLine(alignedX, 0.0f, alignedX, static_cast<float>(kRulerHeight), 1.0f);
                const auto label = formatTimecode(snappedTime, frameRate);
                g.drawText(label, static_cast<int>(std::floor(alignedX)) + 2, 0, 80, kRulerHeight,
                           juce::Justification::topLeft, false);
                lastLabelX = alignedX;
            } else {
                g.setColour(juce::Colour::fromRGBA(60, 60, 60, 160));
                const float y0 = static_cast<float>(kRulerHeight) * (2.0f / 3.0f);
                g.drawLine(alignedX, y0, alignedX, static_cast<float>(kRulerHeight), 1.0f);
            }
        } else {
            g.setColour(juce::Colour::fromRGBA(60, 60, 60, 160));
            const float y0 = static_cast<float>(kRulerHeight) * (2.0f / 3.0f);
            g.drawLine(alignedX, y0, alignedX, static_cast<float>(kRulerHeight), 1.0f);
        }
    }

    g.setColour(juce::Colour::fromRGBA(60, 60, 60, 180));
    g.drawLine(0.0f, static_cast<float>(kRulerHeight - 1), width, static_cast<float>(kRulerHeight - 1), 1.0f);
}
