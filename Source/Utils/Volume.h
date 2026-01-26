#pragma once

#include <juce_dsp/juce_dsp.h>

/// Utility for gain handling and conversions.
class Volume {
public:
    /// Maximum linear gain.
    inline static float MAX_GAIN = 4.0f; // 12.0 db

    /// Create a volume with a clamped linear gain.
    /// @param linearGain initial linear gain
    explicit Volume(float linearGain) {
        if (linearGain > MAX_GAIN) {
            linearGain = MAX_GAIN;
        }
        volume.setGainLinear(linearGain);
    }

    /// get linear number from db number must not be used for -infinity
    /// to get -infinity, use getDbFromLinear(0.0)
    // static float getLinearGainFromDb(float dbGain) {
    //     auto decibelUtil = juce::Decibels<float>();
    //     return getGainLinear();
    // }

    /// Convert a linear gain to decibels (not for -infinity).
    /// @param dbGain linear gain value
    static float getDbFromLinear(float dbGain) {
        juce::dsp::Gain<float> tmpVolume;
        tmpVolume.setGainDecibels(dbGain);
        return tmpVolume.getGainDecibels();
    }
private:
    juce::dsp::Gain<float> volume;
};
