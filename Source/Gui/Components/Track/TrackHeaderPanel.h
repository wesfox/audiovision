#pragma once

#include <JuceHeader.h>
#include "Core/Track/Track.h"
#include "Core/Edit/Edit.h"

class TrackHeader;

class TrackHeaderPanel : public juce::Component {
public:
    TrackHeaderPanel(Edit& edit);

    void resized() override;

    void paint(juce::Graphics& g) override;
private:
    Edit& edit;
    std::vector<std::shared_ptr<Track>>& tracks;

    std::map<String, std::shared_ptr<TrackHeader>> trackHeaderComponents;
};
