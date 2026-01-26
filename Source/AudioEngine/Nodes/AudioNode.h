#pragma once

#include <JuceHeader.h>

#include <atomic>
#include <map>

#include "AudioEngine/Parameters/ParameterFactory.h"

/// Base audio node with parameter binding and JUCE boilerplate.
class AudioNode : public juce::AudioProcessor {
public:
    AudioNode() = default;
    ~AudioNode() override = default;

    /// Bind parameter pointers for this node (call after construction).
    /// @param params map of parameter keys to raw parameter values
    void bindParameters(const std::map<ParameterKey, std::atomic<float>*>& params)
    {
        parameters = params;
    }

protected:
    /// Read a bound parameter pointer by key (returns nullptr if missing).
    /// @param key parameter identifier
    std::atomic<float>* getParameter(ParameterKey key) const
    {
        const auto it = parameters.find(key);
        return (it != parameters.end()) ? it->second : nullptr;
    }

    /// JUCE AudioProcessor no-op overrides required by the base interface.
    void prepareToPlay(double, int) override {}
    void releaseResources() override {}
    bool isBusesLayoutSupported(const BusesLayout&) const override { return true; }

    double getTailLengthSeconds() const override { return 0.0; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override {}
    void setStateInformation(const void*, int) override {}
    bool hasEditor() const override { return false; }
    AudioProcessorEditor* createEditor() override { return nullptr; }

private:
    std::map<ParameterKey, std::atomic<float>*> parameters;
};
