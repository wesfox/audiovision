#pragma once

#include <juce_dsp/juce_dsp.h>

// ------------------------ Volume ------------------------



class Volume {
public:
    inline static float MAX_GAIN = 4.0f; // 12.0 db

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

    /// get linear from db must not be used for -infinity
    static float getDbFromLinear(float dbGain) {
        juce::dsp::Gain<float> tmpVolume;
        tmpVolume.setGainDecibels(dbGain);
        return tmpVolume.getGainDecibels();
    }
private:
    juce::dsp::Gain<float> volume;
};