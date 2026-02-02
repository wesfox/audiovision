#pragma once

#include <juce_graphics/juce_graphics.h>

#include "Gui/Video/VideoFrameBuffer.h"
#include "VideoBackend.h"

/// Frame-accurate backend using AVAssetReader.
class VideoBackend_AVAssetReader : public VideoBackend,
                                   private juce::Thread {
public:
    VideoBackend_AVAssetReader();
    ~VideoBackend_AVAssetReader() override;

    void loadFile(const juce::File& file) override;
    void setPlayheadSeconds(double seconds) override;
    void setFrameRate(double frameRate) override;
    double getPlayheadSeconds() const override;
    void play() override;
    void stop() override;
    bool isReady() const override;
    bool isPlaying() const override;
    juce::Image getCurrentFrameImage() override;
    int64_t getLastFrameIndex() const override;
    double getNominalFrameRate() const override;

private:
    void run() override;
    void resetReader(double startSeconds);
    bool decodeNextFrame();
    int64_t frameIndexForSeconds(double seconds) const;

    static constexpr size_t kBufferCapacity = 20;

    juce::CriticalSection lock;
    void* impl = nullptr;
    VideoFrameBuffer frameBuffer { kBufferCapacity };
    juce::Image lastFrame;
    double targetSeconds = 0.0;
    double frameRate = 0.0;
    double nominalFrameRate = 0.0;
    double readerStartSeconds = 0.0;
    double lastDecodedSeconds = 0.0;
    bool ready = false;
    bool playing = false;
    bool prefillWhilePaused = true;
};
