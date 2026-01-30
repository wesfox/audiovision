#pragma once

#include <JuceHeader.h>

#include "../../Common/Containers/PrimaryContainer.h"
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
    RoundedButton addSendButton;
};
