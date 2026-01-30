#pragma once

#include <JuceHeader.h>

#include "Core/Edit/Edit.h"

/// Adjusts waveform display settings.
class WaveformCommands {
public:
    /// Create the waveform command target.
    /// @param edit edit owning the view state
    /// Create the waveform command target.
    /// @param edit edit owning the view state
    explicit WaveformCommands(Edit& edit);

    void getAllCommands(juce::Array<juce::CommandID>& commands);
    void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result);
    bool perform(const juce::ApplicationCommandTarget::InvocationInfo& info);
    bool handlesCommand(juce::CommandID commandID) const;

private:
    void applyScaleDelta(float delta);

    Edit& edit;
};
