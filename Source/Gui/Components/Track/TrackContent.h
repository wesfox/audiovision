#pragma once

#include <JuceHeader.h>

#include "Core/Edit/Edit.h"
#include "AudioClipComponent.h"

class TrackContent : public juce::Component,
                     public juce::ValueTree::Listener {
public:
    TrackContent(Edit& edit, std::shared_ptr<Track> track);
    ~TrackContent() override;

    void updateLayout();
    void resized() override;
    void paint(juce::Graphics& g) override;
    void valueTreePropertyChanged(juce::ValueTree& tree, const juce::Identifier& property) override;

private:
    void applyWaveformScale();

    Edit& edit;
    std::shared_ptr<Track> track;
    std::vector<std::unique_ptr<AudioClipComponent>> clipComponents;
};
