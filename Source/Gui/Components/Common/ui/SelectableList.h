#pragma once

#include <JuceHeader.h>

/// Simple combo box wrapper for shared look-and-feel styling.
class SelectableList : public juce::Component {
public:
    SelectableList();
    ~SelectableList() override;

    void resized() override;

    /// Replace items with the provided list.
    /// @param items list of {name, id}
    void setItems(const std::map<juce::String, int>& items);

    /// Access the underlying combo box.
    juce::ComboBox& getComboBox() { return comboBox; }

private:
    class SelectableListLookAndFeel;

    juce::ComboBox comboBox;
    std::unique_ptr<SelectableListLookAndFeel> lookAndFeel;
};
