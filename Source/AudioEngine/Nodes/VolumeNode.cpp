#include "VolumeNode.h"

VolumeNode::VolumeNode(const Transport* transport):transport(transport)
{
    setPlayConfigDetails(2, 2, 48000.0, 512);
}

void VolumeNode::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) {
    buffer.applyGain(1.0f); // Apply unity gain
}
