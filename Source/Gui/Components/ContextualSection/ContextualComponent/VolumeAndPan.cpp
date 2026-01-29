#include "VolumeAndPan.h"

#include "Gui/Style/Font.h"

class VolumeAndPan::LabelledKnob : public juce::Component {
public:
    explicit LabelledKnob(juce::String newLabel) : label(std::move(newLabel)) {
        addAndMakeVisible(fader);
    }

    void resized() override {
        auto bounds = getLocalBounds();
        bounds.removeFromBottom(kLabelHeight);
        auto knobBounds = bounds.withSizeKeepingCentre(50, 50);
        fader.setBounds(knobBounds);
    }

    void paint(juce::Graphics& g) override {
        auto labelBounds = getLocalBounds().removeFromBottom(kLabelHeight);
        g.setColour(juce::Colour::fromString("#FF2F2C3F"));
        g.setFont(Fonts::p(11.0f, Fonts::Weight::Regular));
        g.drawText(label, labelBounds, juce::Justification::centred, false);
    }

private:
    static constexpr int kLabelHeight = 16;

    RotaryFader fader;
    juce::String label;
};

VolumeAndPan::VolumeAndPan()
    : stereoLinkToggle("StereoLink",
                       juce::Colour::fromString("#FFEDEBF7"),
                       juce::Colours::black.withAlpha(0.5f)) {
    setTitle("Volume & pan");

    stereoLinkToggle.setToggledColor(juce::Colour::fromString("#B0C8D9B8"),
                                     juce::Colours::black.withAlpha(0.9f));
    addAndMakeVisible(stereoLinkToggle);

    const std::array<juce::String, 6> labels = { "Width", "Focus", "Center", "Front", "Rear", "LFE" };
    for (size_t index = 0; index < labels.size(); ++index) {
        knobs[index] = std::make_unique<LabelledKnob>(labels[index]);
        addAndMakeVisible(*knobs[index]);
    }

    mainVolume = std::make_unique<LabelledKnob>("Main Volume");
    addAndMakeVisible(*mainVolume);
}

VolumeAndPan::~VolumeAndPan() = default;

void VolumeAndPan::resized() {
    auto bounds = getContainerBounds().reduced(8, 8);
    const int toggleHeight = 24;
    const int gap = 8;

    auto toggleBounds = bounds.removeFromTop(toggleHeight);
    stereoLinkToggle.setBounds(toggleBounds.removeFromLeft(120));

    bounds.removeFromTop(gap);
    const int rightColumnWidth = bounds.getWidth() / 4;
    auto rightColumn = bounds.removeFromRight(rightColumnWidth);

    juce::Grid grid;
    grid.templateRows = { 1_fr, 1_fr };
    grid.templateColumns = { 1_fr, 1_fr, 1_fr };
    grid.setGap(juce::Grid::Px(gap));

    for (const auto& knob : knobs) {
        grid.items.add(juce::GridItem(*knob));
    }

    grid.performLayout(bounds);
    if (mainVolume != nullptr) {
        mainVolume->setBounds(rightColumn);
    }
}
