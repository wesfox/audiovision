#include "TrackHeader.h"

#include "Core/Track/Track.h"

#include <algorithm>

// ------------------------ MainComponent Implementation ------------------------

TrackHeader::TrackHeader(Track& track, SelectionManager& selectionManager)
    : track(track),
      selectionManager(selectionManager) {
    trackId = track.getId();
    isSelected = selectionManager.isSelected(trackId);
    selectionManager.addListener(this);
    // defining properties from track
    if (const auto outputTrack = track.getOutput().lock())
        outputTrackName = outputTrack->getName();
    else
        outputTrackName = "";

    // trackColour
    trackColour = track.getColour();

    // trackName (editable)
    trackName = std::make_unique<EditableText>("", juce::Colour(0xFFF0EEFA), juce::Colour(0xFF2F2C3F));
    trackName->setContent(track.getName());
    addAndMakeVisible(trackName.get());

    selector = std::make_unique<SelectableList>();
    selector->setItems({ { "waveform", 1 }, { "volume", 2 } });
    selector->getComboBox().setSelectedId(1, juce::dontSendNotification);
    addAndMakeVisible(selector.get());

    // toggle buttons
    armedToggle = std::make_unique<ArmedToggle>();
    inputMonitoringToggle  = std::make_unique<InputMonitoringToggle>();
    soloToggle = std::make_unique<SoloToggle>();
    activeToggle  = std::make_unique<MuteToggle>();

    addAndMakeVisible(armedToggle.get());
    addAndMakeVisible(inputMonitoringToggle.get());
    addAndMakeVisible(soloToggle.get());
    addAndMakeVisible(activeToggle.get());

    // left vertical line defined in paint()
    // trackOutputName defined in paint()
}

TrackHeader::~TrackHeader() {
    selectionManager.removeListener(this);
}

void TrackHeader::resized() {
    const auto b = getLocalBounds();

    //////////////////// Top line, with track name and toggles
    /// Top Line
    auto topLine = b.withTrimmedTop(6).removeFromTop(24).withTrimmedLeft(12);

    /// TrackName
    trackName->setBounds(topLine.removeFromLeft(80));

    /// Toggles
    auto grid = juce::Grid();
    grid.templateColumns = { 1_fr, 1_fr, 1_fr,  1_fr };
    grid.templateRows = { 1_fr };

    grid.items.add(juce::GridItem(*armedToggle),
               juce::GridItem(*inputMonitoringToggle),
               juce::GridItem(*soloToggle),
               juce::GridItem(*activeToggle));
    grid.setGap(2_px);

    grid.performLayout (topLine.removeFromRight(24*4+8+6).reduced(4,0));

    ////////////////////

    auto selectorLine = b.withTrimmedTop(40).removeFromTop(20).withTrimmedLeft(12);
    selector->setBounds(selectorLine.removeFromLeft(100));
}

void TrackHeader::paint(juce::Graphics &g) {
    // set background
    auto b = getLocalBounds().reduced(0,1);
    g.setColour(juce::Colour::fromRGB(255,255,255));
    g.fillRoundedRectangle(b.toFloat(), 8);
    g.setColour(juce::Colour::fromRGB(16*14+4,16*14+1,16*14+14));
    g.drawRoundedRectangle(b.toFloat(), 8, 1);

    if (isSelected) {
        g.setColour(juce::Colour::fromString("#FFE0CCDD"));
        g.fillRoundedRectangle(b.toFloat(), 8);
    }

    // add padding
    b.removeFromTop(6);
    b.removeFromBottom(6);

    // set left rectangle
    auto colorRect = b.removeFromLeft(12);
    colorRect.reduce(4,0);
    g.setColour(trackColour);
    g.fillRoundedRectangle(colorRect.toFloat(), 2.0f);

    // set output
    if (outputTrackName.isNotEmpty()){
        g.setColour(juce::Colours::black);
        const auto outputTrackNameBounds = getLocalBounds().removeFromRight(100).removeFromBottom(100);
        g.drawFittedText(String::fromUTF8("→ ") + outputTrackName, outputTrackNameBounds, Justification::centred, 2);
    }
}

void TrackHeader::mouseDown(const juce::MouseEvent& event) {
    if (!event.mods.isLeftButtonDown()) {
        return;
    }
    if (event.mods.isCommandDown()) {
        selectionManager.toggleTrackSelection(trackId);
        return;
    }
    if (event.mods.isShiftDown()) {
        selectionManager.extendSelectionToTrack(trackId);
        return;
    }
    selectionManager.setSelection({ trackId });
}

void TrackHeader::setTrackName(const String &name) const {
    trackName->setName(name);
}

void TrackHeader::selectionChanged() {
    const bool nextSelected = selectionManager.isSelected(trackId);
    if (nextSelected == isSelected) {
        return;
    }
    isSelected = nextSelected;
    repaint();
}
