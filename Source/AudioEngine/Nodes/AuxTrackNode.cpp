#include "AuxTrackNode.h"

AuxTrackNode::AuxTrackNode(const Transport* transport):transport(transport)
{
    setPlayConfigDetails(2, 2, 48000.0, 512);
}

void AuxTrackNode::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    // Return buffer directly without any processing
}
