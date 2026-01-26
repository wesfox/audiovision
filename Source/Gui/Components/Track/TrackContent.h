#pragma once

#include <JuceHeader.h>

#include "Core/Edit/Edit.h"
#include "AudioClipComponent.h"

class TrackContent : public juce::Component {
public:
    TrackContent(const Edit& edit, std::shared_ptr<Track> track);

    void resized() override;
    void paint(juce::Graphics& g) override;

private:
    const Edit& edit;
    std::shared_ptr<Track> track;
    std::vector<std::unique_ptr<AudioClipComponent>> clipComponents;
};
