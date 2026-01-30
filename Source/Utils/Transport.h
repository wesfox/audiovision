#pragma once

#include <atomic>

/// Thread-safe playback transport state.
class Transport
{
public:
    /// Initialise transport with a sample rate.
    /// @param sr sample rate
    void prepare(double sr)
    {
        sampleRate = sr;
        currentSample.store(0);
    }

    /// Start playback.
    void play()
    {
        playing.store(true);
    }

    /// Stop playback.
    void stop()
    {
        playing.store(false);
    }

    /// Reset transport position to zero.
    void rewind()
    {
        currentSample.store(0);
    }

    /// True when playback is active.
    bool isPlaying() const
    {
        return playing.load();
    }

    /// advance (or go back) from numSample
    /// @param numSamples number of sample to move (can be negative, move backward)
    void advance(int64 numSamples)
    {
        if (currentSample.load() <= -numSamples){
            currentSample.store(0LL);
        }else{
            currentSample.fetch_add(numSamples);
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

    /// Current playhead position in samples.
    int64_t getCursorPosition() const {
        return currentSample.load();
    }

    /// Set playhead position in samples.
    /// @param sample new playhead sample position
    void setCursorPosition(int64_t sample) {
        currentSample.store(sample);
    }

    /// Current playhead position in milliseconds.
    double getMs() const {
        return static_cast<double>(currentSample.load()) / static_cast<double>(sampleRate.load()) * 1000.0;
    }

private:
    std::atomic<int64_t> currentSample{ 0 };
    std::atomic<bool> playing{ false };
    std::atomic<int> sampleRate = 48000;
    std::atomic<int> blockSize = 512;
};
