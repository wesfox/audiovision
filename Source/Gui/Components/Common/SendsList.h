#pragma once

#include <JuceHeader.h>

#include "Gui/Components/Common/PrimaryContainer.h"
#include "ReverbComponent.h"

/// Displays a list of send components stacked vertically.
class SendsList : public PrimaryContainer {
public:
    SendsList();

    void resized() override;

    static constexpr int kHeight = 211;

private:
    ReverbComponent reverbA;
    ReverbComponent reverbB;
};
