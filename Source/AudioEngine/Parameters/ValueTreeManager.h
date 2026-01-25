#pragma once

#include <JuceHeader.h>

#include <atomic>
#include <memory>
#include <map>
#include <vector>

#include "ParameterFactory.h"

class GraphManager;

class ValueTreeManager {
public:
    struct PluginParamInfo {
        juce::String id;
        juce::String name;
        juce::AudioProcessorParameter* param = nullptr;
        juce::RangedAudioParameter* rangedParam = nullptr;
    };

    ValueTreeManager();

    void buildForGraph(const GraphManager& graphManager);

    std::atomic<float>* getRawParameterValue(const juce::String& trackId,
                                             const juce::String& paramName) const;
    std::atomic<float>* getRawParameterValue(const juce::String& trackId,
                                             ParameterKey key) const;
    std::map<ParameterKey, std::atomic<float>*> buildParamMap(
        const juce::String& trackId,
        const std::vector<ParameterKey>& keys) const;
    float getParameterValue(const juce::String& trackId, ParameterKey key) const;
    void setParameterValue(const juce::String& trackId, ParameterKey key, float value) const;
    void registerPluginParameters(const juce::String& pluginId, juce::AudioProcessor& processor);
    const std::vector<PluginParamInfo>* getPluginParameters(const juce::String& pluginId) const;
    float getPluginParameterValue(const juce::String& pluginId, const juce::String& paramId) const;
    void setPluginParameterValue(const juce::String& pluginId,
                                 const juce::String& paramId,
                                 float value) const;

    juce::String makeParamId(const juce::String& trackId,
                             const juce::String& paramName) const;
    juce::String makeParamId(const juce::String& trackId,
                             ParameterKey key) const;

    juce::AudioProcessorValueTreeState* getState() const { return apvts.get(); }

private:
    class ParameterHost final : public juce::AudioProcessor {
    public:
        ParameterHost();

        const juce::String getName() const override { return "ValueTreeManagerHost"; }
        void prepareToPlay(double, int) override {}
        void releaseResources() override {}
        bool isBusesLayoutSupported(const BusesLayout& layouts) const override { return true; }
        void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override {}

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
    };

    ParameterHost host;
    std::unique_ptr<juce::AudioProcessorValueTreeState> apvts;
    std::map<juce::String, std::vector<PluginParamInfo>> pluginParams;
};
