#pragma once

#include <JuceHeader.h>

class Edit;

/// Import and export edits to disk.
class EditSerializer {
public:
    /// Load an edit from a file.
    /// @param file input file
    static std::shared_ptr<Edit> importFromFile(const juce::File& file);

    /// Save an edit to a file.
    /// @param edit edit to export
    /// @param file output file or directory
    static bool exportToFile(const Edit &edit, const juce::File &file);
};
