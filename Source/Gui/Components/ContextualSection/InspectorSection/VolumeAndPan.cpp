#include "VolumeAndPan.h"

#include "Gui/Style/Font.h"

class VolumeAndPan::LabelledKnob : public juce::Component {
public:
    explicit LabelledKnob(juce::String newLabel) : label(std::move(newLabel)) {
        fader.setLabel(label);
        addAndMakeVisible(fader);
    }

    void resized() override {
        auto knobBounds = getLocalBounds().withSizeKeepingCentre(50, 70);
        fader.setBounds(knobBounds);
    }

private:
    RotaryFader fader;
    juce::String label;
};

class VolumeAndPan::MainVolumeControl : public juce::Component {
public:
    MainVolumeControl() {
        addAndMakeVisible(fader);
    }

    void resized() override {
        auto bounds = getLocalBounds();
        bounds.removeFromBottom(kLabelHeight);
        const int minHeight = 120;
        const int maxHeight = 250;
        auto faderBounds = bounds.withSizeKeepingCentre(bounds.getWidth(),
                                                        std::clamp(bounds.getHeight(), minHeight, maxHeight));
        faderBounds.setY(getLocalBounds().getY());
        fader.setBounds(faderBounds);
    }

    void paint(juce::Graphics& g) override {
        auto labelBounds = getLocalBounds().removeFromBottom(kLabelHeight);
        g.setColour(juce::Colour::fromString("#FF2F2C3F"));
        g.setFont(Fonts::p(11.0f, Fonts::Weight::Regular));
        g.drawText("Main Volume", labelBounds, juce::Justification::centred, false);
    }

private:
    static constexpr int kLabelHeight = 16;

    VerticalFader fader;
};

VolumeAndPan::VolumeAndPan()
    : stereoLinkToggle("StereoLink", juce::Colour::fromString("#FFE7E1FB")) {
    setTitle("Volume & pan");

    addAndMakeVisible(stereoLinkToggle);

    const std::array<juce::String, 6> labels = { "Width", "Focus", "Center", "Front", "Rear", "LFE" };
    for (size_t index = 0; index < labels.size(); ++index) {
        knobs[index] = std::make_unique<LabelledKnob>(labels[index]);
        addAndMakeVisible(*knobs[index]);
    }

    mainVolume = std::make_unique<MainVolumeControl>();
    addAndMakeVisible(*mainVolume);
}

VolumeAndPan::~VolumeAndPan() = default;

void VolumeAndPan::resized() {
    auto bounds = getContainerBounds().reduced(8, 8);
    const int toggleHeight = 24;
    const int gap = 8;

    auto toggleBounds = bounds.removeFromTop(toggleHeight);
    stereoLinkToggle.setBounds(toggleBounds.removeFromLeft(120));

    const int rightColumnWidth = bounds.getWidth() / 4;
    bounds.removeFromRight(rightColumnWidth);
    auto rightColumn = getLocalBounds().removeFromRight(rightColumnWidth).withTrimmedTop(4);

    juce::Grid grid;
    grid.templateRows = { 1_fr, 1_fr };
    grid.templateColumns = { 1_fr, 1_fr, 1_fr };
    grid.setGap(juce::Grid::Px(gap));

    for (const auto& knob : knobs) {
        grid.items.add(juce::GridItem(*knob));
    }

    grid.performLayout(bounds);
    if (mainVolume != nullptr) {
        const int maxHeight = 190 + 16;
        auto clampedRight = rightColumn.withSizeKeepingCentre(rightColumn.getWidth(),
                                                              std::min(rightColumn.getHeight(), maxHeight));
        mainVolume->setBounds(clampedRight);
    }
}
