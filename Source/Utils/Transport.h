#pragma once

#include <atomic>

// ------------------------ Transport ------------------------

class Transport
{
public:
    void prepare(double sr)
    {
        sampleRate = sr;
        currentSample.store(0);
    }

    void play()
    {
        playing.store(true);
    }

    void stop()
    {
        playing.store(false);
    }

    /// set transport to 0
    void rewind()
    {
        currentSample.store(0);
    }

    bool isPlaying() const
    {
        return playing.load();
    }

    /// advance (or go back) from numSample
    /// @param numSamples number of sample to move (can be negative, move backward)
    void advance(int numSamples)
    {
        if (playing.load())
            currentSample.fetch_add(numSamples);
    }

    double getSampleRate() const {
        return sampleRate.load();
    }

    int getCurrentBlockSize() const {
        return blockSize.load();
    }

    void setCurrentBlockSize(int newBlockSize) {
        blockSize.store(newBlockSize);
    }

    int64_t getCursorPosition() const {
        return currentSample.load();
    }

    double getMs() const {
        return static_cast<double>(currentSample.load()) / static_cast<double>(sampleRate.load()) * 1000.0;
    }

private:
    std::atomic<int64_t> currentSample{ 0 };
    std::atomic<bool> playing{ false };
    std::atomic<int> sampleRate = 48000;
    std::atomic<int> blockSize = 512;
};
