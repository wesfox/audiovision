#include "Edit.h"

Edit::Edit()
    : transport(),
      projectName(""),
      automationManager(),
      videoStartFrame(0),
      video(),
      videoSyncOffset(0)
{
    audioGraph = std::make_shared<juce::AudioProcessorGraph>();
    audioOutputEngine = std::make_unique<AudioOutputEngine>(audioGraph);
    audioOutputEngine->initialise(ChannelsFormat::Mono, 48000.0, 512);
}
