#pragma once

#include <JuceHeader.h>

#include <atomic>
#include <memory>
#include <map>
#include <vector>

#include "ParameterFactory.h"

class GraphManager;

/// Own app parameters and provide access helpers for UI and nodes.
class ValueTreeManager {
public:
    /// Plugin parameter view for UI and automation helpers.
    struct PluginParamInfo {
        juce::String id;
        juce::String name;
        juce::AudioProcessorParameter* param = nullptr;
        juce::RangedAudioParameter* rangedParam = nullptr;
    };

    ValueTreeManager();

    /// Build app-owned parameters for the current graph.
    /// @param graphManager graph manager providing nodes
    void buildForGraph(const GraphManager& graphManager);

    /// Get a raw parameter pointer by track id and name.
    /// @param trackId track id owning the parameter
    /// @param paramName parameter name
    std::atomic<float>* getRawParameterValue(const juce::String& trackId,
                                             const juce::String& paramName) const;

    /// Get a raw parameter pointer by track id and key.
    /// @param trackId track id owning the parameter
    /// @param key parameter identifier
    std::atomic<float>* getRawParameterValue(const juce::String& trackId,
                                             ParameterKey key) const;

    /// Build a map of raw parameter pointers for a node.
    /// @param trackId track id owning the parameters
    /// @param keys parameter identifiers to fetch
    std::map<ParameterKey, std::atomic<float>*> buildParamMap(
        const juce::String& trackId,
        const std::vector<ParameterKey>& keys) const;

    /// Read a parameter value in real units.
    /// @param trackId track id owning the parameter
    /// @param key parameter identifier
    float getParameterValue(const juce::String& trackId, ParameterKey key) const;

    /// Set a parameter value in real units.
    /// @param trackId track id owning the parameter
    /// @param key parameter identifier
    /// @param value new value in real units
    void setParameterValue(const juce::String& trackId, ParameterKey key, float value) const;

    /// Register plugin parameters for lookup by plugin id.
    /// @param pluginId unique plugin id
    /// @param processor plugin processor instance
    void registerPluginParameters(const juce::String& pluginId, juce::AudioProcessor& processor);

    /// Access cached plugin parameters by plugin id.
    /// @param pluginId unique plugin id
    const std::vector<PluginParamInfo>* getPluginParameters(const juce::String& pluginId) const;

    /// Read a plugin parameter value in real units.
    /// @param pluginId unique plugin id
    /// @param paramId parameter id or name
    float getPluginParameterValue(const juce::String& pluginId, const juce::String& paramId) const;

    /// Set a plugin parameter value in real units.
    /// @param pluginId unique plugin id
    /// @param paramId parameter id or name
    /// @param value new value in real units
    void setPluginParameterValue(const juce::String& pluginId,
                                 const juce::String& paramId,
                                 float value) const;

    /// Build a stable app parameter id for APVTS.
    /// @param trackId track id owning the parameter
    /// @param paramName parameter name
    juce::String makeParamId(const juce::String& trackId,
                             const juce::String& paramName) const;

    /// Build a stable app parameter id for APVTS.
    /// @param trackId track id owning the parameter
    /// @param key parameter identifier
    juce::String makeParamId(const juce::String& trackId,
                             ParameterKey key) const;

    /// Access the underlying APVTS.
    juce::AudioProcessorValueTreeState* getState() const { return apvts.get(); }

private:
    /// Minimal AudioProcessor host for APVTS (no-op overrides are required by JUCE).
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
