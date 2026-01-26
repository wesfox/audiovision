#include "TrackHeader.h"

#include "Core/Track/Track.h"

// ------------------------ MainComponent Implementation ------------------------

TrackHeader::TrackHeader(Track& track): track(track) {
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

    auto toggleBackgroundColour = juce::Colour(0xFFE7E1FB);
    auto toggleTextColor = juce::Colour(0xFF000000);
    // toggle buttons
    armedToggle = std::make_unique<ToggleTextButton>("R", toggleBackgroundColour, toggleTextColor);
    armedToggle->setToggledColor(juce::Colour(0xFFFF8787), toggleTextColor);

    inputMonitoringToggle  = std::make_unique<ToggleTextButton>("I", toggleBackgroundColour, toggleTextColor);
    inputMonitoringToggle->setToggledColor(juce::Colour(0xFF68C39A), toggleTextColor);

    soloToggle = std::make_unique<ToggleTextButton>("S", toggleBackgroundColour, toggleTextColor);
    soloToggle->setToggledColor(juce::Colour(0xFFFFDFAC), toggleTextColor);

    activeToggle  = std::make_unique<ToggleTextButton>("ON", juce::Colour(0xFFD9E9DB), toggleTextColor);
    activeToggle->setToggledColor(juce::Colour(0xFF7DE38F), toggleTextColor);
    activeToggle->setToggled(true);

    addAndMakeVisible(armedToggle.get());
    addAndMakeVisible(inputMonitoringToggle.get());
    addAndMakeVisible(soloToggle.get());
    addAndMakeVisible(activeToggle.get());

    // left vertical line defined in paint()
    // trackOutputName defined in paint()
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
}

void TrackHeader::paint(juce::Graphics &g) {
    // set background
    auto b = getLocalBounds().reduced(0,1);
    g.setColour(juce::Colour::fromRGB(255,255,255));
    g.fillRoundedRectangle(b.toFloat(), 8);
    g.setColour(juce::Colour::fromRGB(16*14+4,16*14+1,16*14+14));
    g.drawRoundedRectangle(b.toFloat(), 8, 1);

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

void TrackHeader::setTrackName(const String &name) const {
    trackName->setName(name);
}
