#pragma once

#include <JuceHeader.h>

/// Helpers for converting between samples and timecode frames.
namespace Timecode {
/// Convert sample index to a frame index with rounding.
/// @param sample sample index to convert
/// @param sampleRate audio sample rate in Hz
/// @param frameRate timeline frame rate in fps
inline int64 samplesToFrames(int64 sample, double sampleRate, double frameRate) {
    if (sampleRate <= 0.0 || frameRate <= 0.0) {
        return 0;
    }
    const auto frames = static_cast<double>(sample) * frameRate / sampleRate;
    return static_cast<int64>(std::llround(frames));
}

/// Convert a frame index to a timecode string.
/// @param frames frame index to format
/// @param frameRate timeline frame rate in fps
inline juce::String formatTimecodeFrames(int64 frames, double frameRate) {
    if (frameRate <= 0.0) {
        // Frame rate must be positive for timecode formatting.
        jassert(false);
        frameRate = 30.0;
    }

    const double fps = frameRate;
    const double totalSecondsExact = static_cast<double>(frames) / fps;
    const int totalSeconds = static_cast<int>(std::floor(totalSecondsExact));
    const int hours = totalSeconds / 3600;
    const int minutes = (totalSeconds / 60) % 60;
    const int secs = totalSeconds % 60;
    const double fractionalFrames = static_cast<double>(frames) - (static_cast<double>(totalSeconds) * fps);
    int frameNumber = static_cast<int>(std::round(fractionalFrames));
    const int maxFrame = static_cast<int>(std::floor(fps));
    if (frameNumber >= maxFrame) {
        frameNumber = 0;
    }

    return juce::String::formatted("%02d:%02d:%02d:%02d", hours, minutes, secs, frameNumber);
}
}
