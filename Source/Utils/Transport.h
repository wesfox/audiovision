#pragma once

#include <atomic>
#include <optional>

static constexpr int NEVER = -1;

/// Thread-safe playback transport state.
class Transport
{
public:
    /// Initialise transport with a sample rate.
    /// @param sr sample rate
    void prepare(double sr)
    {
        sampleRate = sr;
        playheadSample.store(0);
    }

    /// Start playback.
    void play()
    {
        play(std::nullopt);
    }

    /// Start playback looping between startSample and endSample.
    /// @param endSample is end sample
    /// @param isLooping do we need to loop
    void play(int64_t endSample, bool isLooping)
    {
        playStartSample.store(playheadSample.load());
        playEndSample.store(endSample);
        looping.store(isLooping);
        playing.store(true);
        playSelection.store(true);

        // ensure reseting other values
        hasStopped.store(false);
    }

    /// Start playback with an optional end sample.
    /// @param endSample optional sample where playback should stop after passing it
    void play(std::optional<int64_t> endSample)
    {
        hasStopped.store(false);
        if (endSample.has_value()) {
            playSelection.store(true);
            playEndSample.store(endSample.value());
        } else {
            playEndEnabled.store(false);
        }
        playing.store(true);
    }

    /// Stop playback.
    void stop()
    {
        hasStopped.store(true);
        playSelection.store(false);
        playing.store(false);
    }

    /// Reset transport position to zero.
    void rewind()
    {
        playheadSample.store(0);
    }

    /// True when playback is active.
    bool isPlaying() const
    {
        return playing.load();
    }

    /// Advance (or go back) from numSample.
    /// @param numSamples number of sample to move (can be negative, move backward)
    void advance(int64 numSamples)
    {
        if (playheadSample.load() <= -numSamples){
            playheadSample.store(0LL);
        }else{
            playheadSample.fetch_add(numSamples);
        }

        if (playSelection.load() && playheadSample.load() > playEndSample.load()) {
            if (looping.load()){
                playheadSample.store(playStartSample.load());
            }
            else{
                playheadSample.store(playEndSample.load()); // remove the overhead
                stop();
            } 
        }
    }

    /// Current sample rate.
    double getSampleRate() const {
        return sampleRate.load();
    }

    /// Current block size.
    int getCurrentBlockSize() const {
        return blockSize.load();
    }

    /// Set the current block size.
    /// @param newBlockSize new block size
    void setCurrentBlockSize(int newBlockSize) {
        blockSize.store(newBlockSize);
    }

    /// Set the playback frame rate.
    /// @param newFrameRate frame rate to use for frame conversions
    void setFrameRate(double newFrameRate) {
        frameRate.store(newFrameRate);
    }

    /// Current playhead position in frames.
    int64_t getCurrentFrame() const {
        const auto fr = frameRate.load();
        if (fr <= 0.0) {
            // Frame rate must be set before requesting frames.
            jassert(false);
            return 0;
        }
        return static_cast<int64_t>(std::llround(
            static_cast<double>(playheadSample.load()) * fr / sampleRate.load()));
    }

    /// Current playhead position in samples.
    int64_t getPlayheadSample() const {
        return playheadSample.load();
    }

    /// Set playhead position in samples.
    /// @param sample new playhead sample position
    void setPlayheadSample(int64_t sample) {
        playheadSample.store(sample);
    }

    /// End sample used for play range.
    int64_t getPlayEndSample() const {
        return playEndSample.load();
    }

    /// True once when playback ended at the range end.
    bool consumeHasStopped() {
        return hasStopped.exchange(false);
    }

    /// Current playhead position in milliseconds.
    double getMs() const {
        return static_cast<double>(playheadSample.load()) / static_cast<double>(sampleRate.load()) * 1000.0;
    }

private:
    std::atomic<int64_t> playheadSample{ 0 };
    std::atomic<int64_t> playEndSample{ 0 };
    std::atomic<int64_t> playStartSample{ 0 };
    std::atomic<bool> playing{ false };
    std::atomic<bool> playEndEnabled{ false };
    std::atomic<bool> hasStopped{ false };
    std::atomic<bool> looping{ false };
    std::atomic<bool> playSelection{ false };
    std::atomic<int> sampleRate = 48000;
    std::atomic<int> blockSize = 512;
    std::atomic<double> frameRate = 0.0;
};
