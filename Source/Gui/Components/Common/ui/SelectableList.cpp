#include "SelectableList.h"

#include "Style/Font.h"

namespace {
constexpr int kCornerRadius = 4;
const juce::Colour kBorderColour = juce::Colour(0xFF4C2C7E);
const juce::Colour kFillColour = juce::Colour(0xFFEFE8FA);
const juce::Colour kTextColour = juce::Colour(0xFF2F2C3F);
}

class SelectableList::SelectableListLookAndFeel : public juce::LookAndFeel_V4 {
public:
    void drawPopupMenuBackground(juce::Graphics& g, int width, int height) override {
        auto bounds = juce::Rectangle<float>(0.0f, 0.0f, static_cast<float>(width),
                                             static_cast<float>(height));
        g.setColour(findColour(juce::PopupMenu::backgroundColourId));
        g.fillRect(bounds);
    }

    void drawComboBox(juce::Graphics& g, int width, int height, bool,
                      int, int, int, int, juce::ComboBox& box) override {
        const auto bounds = juce::Rectangle<float>(0, 0, static_cast<float>(width), static_cast<float>(height));
        g.setColour(kFillColour);
        g.fillRoundedRectangle(bounds, static_cast<float>(kCornerRadius));
        g.setColour(kBorderColour);
        const auto inset = bounds.reduced(0.5f);
        g.drawRoundedRectangle(inset, static_cast<float>(kCornerRadius - 0.5f), 1.0f);

        const float arrowSize = 6.0f;
        const float arrowX = static_cast<float>(width) - 16.0f;
        const float arrowY = static_cast<float>(height) * 0.5f - 2.0f;
        juce::Path arrow;
        arrow.startNewSubPath(arrowX, arrowY);
        arrow.lineTo(arrowX + arrowSize, arrowY);
        arrow.lineTo(arrowX + arrowSize * 0.5f, arrowY + arrowSize);
        arrow.closeSubPath();
        g.fillPath(arrow);

        box.setColour(juce::ComboBox::textColourId, kTextColour);
    }

    void positionComboBoxText(juce::ComboBox& box, juce::Label& label) override {
        label.setBounds(8, 0, box.getWidth() - 28, box.getHeight());
        label.setFont(Fonts::p(14.0f, Fonts::Weight::Regular));
    }

    void drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
                           bool isSeparator, bool isActive, bool isHighlighted,
                           bool isTicked, bool hasSubMenu, const juce::String& text,
                           const juce::String& shortcutKeyText, const juce::Drawable* icon,
                           const juce::Colour* textColour) override {
        juce::LookAndFeel_V4::drawPopupMenuItem(g, area, isSeparator, isActive, isHighlighted,
                                                isTicked, hasSubMenu, text, shortcutKeyText, icon, textColour);
    }
};

SelectableList::SelectableList() {
    lookAndFeel = std::make_unique<SelectableListLookAndFeel>();
    lookAndFeel->setColour(juce::PopupMenu::backgroundColourId, kFillColour);
    lookAndFeel->setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colour(0xFFD9CFF0));
    lookAndFeel->setColour(juce::PopupMenu::textColourId, kTextColour);
    lookAndFeel->setColour(juce::PopupMenu::highlightedTextColourId, kTextColour);
    comboBox.setLookAndFeel(lookAndFeel.get());
    comboBox.setWantsKeyboardFocus(false);
    addAndMakeVisible(comboBox);
}

SelectableList::~SelectableList() {
    comboBox.setLookAndFeel(nullptr);
}

void SelectableList::resized() {
    comboBox.setBounds(getLocalBounds());
}

void SelectableList::setItems(const std::map<juce::String, int>& items) {
    comboBox.clear(juce::dontSendNotification);
    for (const auto& entry : items) {
        comboBox.addItem(entry.first, entry.second);
    }
}
