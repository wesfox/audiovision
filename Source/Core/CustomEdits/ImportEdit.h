#pragma once

#include <JuceHeader.h>

#include "Core/Edit/Edit.h"

class ImportEdit : public juce::Component {
public:
    ImportEdit();
    static std::shared_ptr<Edit> importEdit();

private:
};
