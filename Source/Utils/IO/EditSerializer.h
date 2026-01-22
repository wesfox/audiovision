#pragma once

#include <JuceHeader.h>

class Edit;

class EditSerializer {
public:
    static std::shared_ptr<Edit> importFromFile(const juce::File& file);
    static bool exportToFile(const Edit &edit, const juce::File &file);
};
