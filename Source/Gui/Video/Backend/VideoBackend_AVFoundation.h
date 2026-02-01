#pragma once

#include <juce_graphics/juce_graphics.h>

#include "VideoBackend.h"

/// AVFoundation-backed video playback.
class VideoBackend_AVFoundation : public VideoBackend {
public:
    VideoBackend_AVFoundation();
    ~VideoBackend_AVFoundation() override;

    void loadFile(const juce::File& file) override;
    void setPlayheadSeconds(double seconds) override;
    void setFrameRate(double frameRate) override;
    double getPlayheadSeconds() const override;
    void play() override;
    void stop() override;
    bool isReady() const override;
    bool isPlaying() const override;
    juce::Image getCurrentFrameImage() override;
    void updateFrameAtTime(double seconds);
    int64_t getLastFrameIndex() const override;
    double getNominalFrameRate() const override;

private:
    void* impl = nullptr;
    juce::Image currentFrame;
    double lastFrameSeconds = -1.0;
    int64_t lastFrameIndex = -1;
    double frameRate = 0.0;
    double nominalFrameRate = 0.0;
};
