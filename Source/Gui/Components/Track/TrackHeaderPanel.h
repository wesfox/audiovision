#pragma once

#include <JuceHeader.h>
#include "Core/Track/Track.h"
#include "Core/Edit/Edit.h"
#include "Gui/Utils/SelectionManager.h"

class TrackHeader;

class TrackHeaderPanel : public juce::Component {
public:
    TrackHeaderPanel(Edit& edit, SelectionManager& selectionManager);

    void resized() override;

    void paint(juce::Graphics& g) override;
private:
    Edit& edit;
    SelectionManager& selectionManager;
    std::vector<std::shared_ptr<Track>>& tracks;

    std::map<String, std::shared_ptr<TrackHeader>> trackHeaderComponents;
};
