#include "DebugWatchWindow.h"

#include "Gui/Style/Font.h"

#include <algorithm>
#include <functional>

namespace {
constexpr int kDefaultWidth = 320;
constexpr int kDefaultHeight = 220;
constexpr int kPadding = 8;
constexpr int kLineHeight = 16;
}

class DebugWatchWindow::DebugWatchView : public juce::Component,
                                         private juce::Timer {
public:
    DebugWatchView() {
        startTimerHz(10);
    }

    void onContentHeightChanged(std::function<void()> callback) {
        onContentHeightChangedCallback = std::move(callback);
    }

    int getPreferredHeight() const {
        return contentHeight;
    }

    void paint(juce::Graphics& g) override {
        g.fillAll(juce::Colour::fromRGBA(20, 20, 20, 230));
        g.setColour(juce::Colours::white);
        g.setFont(Fonts::p(12.0f, Fonts::Weight::Regular));

        auto bounds = getLocalBounds().reduced(kPadding);
        for (const auto& line : lines) {
            if (bounds.getHeight() < kLineHeight) {
                break;
            }
            auto row = bounds.removeFromTop(kLineHeight);
            g.drawText(line.name + ": " + line.value, row, juce::Justification::centredLeft, true);
        }
    }

private:
    void timerCallback() override {
        lines = DebugWatchRegistry::get().snapshot();
        const int minHeight = kLineHeight + kPadding * 2;
        const int nextHeight = std::max(minHeight, static_cast<int>(lines.size()) * kLineHeight + kPadding * 2);
        if (nextHeight != contentHeight) {
            contentHeight = nextHeight;
            if (onContentHeightChangedCallback) {
                onContentHeightChangedCallback();
            }
        }
        repaint();
    }

    std::vector<DebugWatchRegistry::WatchLine> lines;
    std::function<void()> onContentHeightChangedCallback;
    int contentHeight = kDefaultHeight;
};

class DebugWatchWindow::DebugWatchContent : public juce::Component {
public:
    DebugWatchContent() {
        viewport.setViewedComponent(&watchView, false);
        viewport.setScrollBarsShown(true, false);
        addAndMakeVisible(viewport);

        watchView.onContentHeightChanged([this]() { updateViewSize(); });
    }

    void resized() override {
        viewport.setBounds(getLocalBounds());
        updateViewSize();
    }

private:
    void updateViewSize() {
        const int viewportWidth = viewport.getWidth();
        if (viewportWidth <= 0) {
            return;
        }
        const int viewWidth = std::max(0, viewportWidth - viewport.getScrollBarThickness());
        const int viewHeight = std::max(watchView.getPreferredHeight(), viewport.getHeight());
        watchView.setSize(viewWidth, viewHeight);
    }

    juce::Viewport viewport;
    DebugWatchView watchView;
};

DebugWatchWindow::DebugWatchWindow()
    : juce::DocumentWindow("Debug Watch",
                           juce::Colours::black,
                           juce::DocumentWindow::allButtons) {
    setUsingNativeTitleBar(true);
    setResizable(true, true);
    setAlwaysOnTop(true);
    setContentOwned(new DebugWatchContent(), true);
    centreWithSize(kDefaultWidth, kDefaultHeight);
    setVisible(false);
}

void DebugWatchWindow::closeButtonPressed() {
    setVisible(false);
}
