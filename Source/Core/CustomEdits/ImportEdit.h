#pragma once

#include <JuceHeader.h>

#include "Core/Edit/Edit.h"

/// Helper for loading an Edit from disk.
class ImportEdit : public juce::Component {
public:
    /// Create an importer.
    ImportEdit();

    /// Load the edit from the default location.
    static std::shared_ptr<Edit> importEdit();

private:
};
