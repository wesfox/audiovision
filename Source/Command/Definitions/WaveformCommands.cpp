#include "WaveformCommands.h"

#include "Command/Commands.h"

namespace {
constexpr float kScaleStep = 0.2f;
constexpr float kMinScale = 0.1f;
}

WaveformCommands::WaveformCommands(Edit& edit)
    : edit(edit) {
}

void WaveformCommands::getAllCommands(juce::Array<juce::CommandID>& commands) {
    commands.add(CommandIds::Waveform::increaseScale);
    commands.add(CommandIds::Waveform::decreaseScale);
}

void WaveformCommands::getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result) {
    if (commandID == CommandIds::Waveform::increaseScale) {
        result.setInfo("Increase Waveform Scale", "Increase waveform amplitude scaling", "View", 0);
    } else if (commandID == CommandIds::Waveform::decreaseScale) {
        result.setInfo("Decrease Waveform Scale", "Decrease waveform amplitude scaling", "View", 0);
    }
}

bool WaveformCommands::perform(const juce::ApplicationCommandTarget::InvocationInfo& info) {
    if (info.commandID == CommandIds::Waveform::increaseScale) {
        applyScaleDelta(kScaleStep);
        return true;
    }
    if (info.commandID == CommandIds::Waveform::decreaseScale) {
        applyScaleDelta(-kScaleStep);
        return true;
    }
    return false;
}

bool WaveformCommands::handlesCommand(juce::CommandID commandID) const {
    return commandID == CommandIds::Waveform::increaseScale
        || commandID == CommandIds::Waveform::decreaseScale;
}

void WaveformCommands::applyScaleDelta(float delta) {
    const float current = edit.getState().getWaveformScale();
    const float next = juce::jmax(kMinScale, current + delta);
    edit.getState().setWaveformScale(next);
}
