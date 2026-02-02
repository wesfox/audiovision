#include "DebugWatchWindow.h"

#include "Gui/Style/Font.h"

namespace {
constexpr int kDefaultWidth = 320;
constexpr int kDefaultHeight = 220;
constexpr int kPadding = 8;
}

class DebugWatchWindow::DebugWatchView : public juce::Component,
                                         private juce::Timer {
public:
    DebugWatchView() {
        startTimerHz(10);
    }

    void paint(juce::Graphics& g) override {
        g.fillAll(juce::Colour::fromRGBA(20, 20, 20, 230));
        g.setColour(juce::Colours::white);
        g.setFont(Fonts::p(12.0f, Fonts::Weight::Regular));

        auto bounds = getLocalBounds().reduced(kPadding);
        const int lineHeight = 16;
        for (const auto& line : lines) {
            if (bounds.getHeight() < lineHeight) {
                break;
            }
            auto row = bounds.removeFromTop(lineHeight);
            g.drawText(line.name + ": " + line.value, row, juce::Justification::centredLeft, true);
        }
    }

private:
    void timerCallback() override {
        lines = DebugWatchRegistry::get().snapshot();
        repaint();
    }

    std::vector<DebugWatchRegistry::WatchLine> lines;
};

DebugWatchWindow::DebugWatchWindow()
    : juce::DocumentWindow("Debug Watch",
                           juce::Colours::black,
                           juce::DocumentWindow::allButtons) {
    setUsingNativeTitleBar(true);
    setResizable(true, true);
    setAlwaysOnTop(true);
    setContentOwned(new DebugWatchView(), true);
    centreWithSize(kDefaultWidth, kDefaultHeight);
    setVisible(true);
    toFront(true);
}

void DebugWatchWindow::closeButtonPressed() {
    setVisible(false);
}
