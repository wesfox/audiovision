#include "SendsComponent.h"

#include "Gui/Style/Font.h"

class SendsComponent::SendKnob : public juce::Component {
public:
    explicit SendKnob(juce::String newLabel) : label(std::move(newLabel)) {
        addAndMakeVisible(fader);
    }

    void resized() override {
        auto bounds = getLocalBounds();
        bounds.removeFromBottom(kLabelHeight);
        fader.setBounds(bounds);
    }

    void paint(juce::Graphics& g) override {
        auto labelBounds = getLocalBounds().removeFromBottom(kLabelHeight);
        g.setColour(juce::Colour::fromString("#FF2F2C3F"));
        g.setFont(Fonts::p(11.0f, Fonts::Weight::Regular));
        g.drawText(label, labelBounds, juce::Justification::centred, false);
    }

    void setLabel(juce::String newLabel) {
        label = std::move(newLabel);
        repaint();
    }

private:
    static constexpr int kLabelHeight = 16;

    RotaryFader fader;
    juce::String label;
};

SendsComponent::SendsComponent()
    : addButton("+") {
    setTitle("Sends");
    addButton.setState(RoundedButton::State::Inactive);
    addAndMakeVisible(addButton);
    addButton.onClick = [this] {
        addSend("new Send");
    };
}

SendsComponent::~SendsComponent() = default;

void SendsComponent::addSend(juce::String sendName) {
    sends.push_back(std::move(sendName));
    refreshKnobs();
    resized();
    repaint();
}

void SendsComponent::resized() {
    auto bounds = getContainerBounds().reduced(8, 8);

    juce::Grid grid;
    grid.templateRows = { 1_fr, 1_fr };
    grid.templateColumns = { 1_fr, 1_fr, 1_fr };
    grid.setGap(juce::Grid::Px(8));

    for (const auto& knob : knobs) {
        grid.items.add(juce::GridItem(*knob));
    }

    grid.items.add(juce::GridItem(addButton).withHeight(30).withAlignSelf(GridItem::AlignSelf::center));
    grid.performLayout(bounds);
}

void SendsComponent::paint(juce::Graphics& g) {
    SecondaryContainer::paint(g);
}

void SendsComponent::refreshKnobs() {
    knobs.clear();

    for (const auto& send : sends) {
        auto knob = std::make_unique<SendKnob>(send);
        addAndMakeVisible(*knob);
        knobs.emplace_back(std::move(knob));
    }
}
