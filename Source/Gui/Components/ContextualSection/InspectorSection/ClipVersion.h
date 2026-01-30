#pragma once

#include <JuceHeader.h>

#include "../../Common/Containers/SecondaryContainer.h"
#include "../../Common/ui/SelectableList.h"

/// Displays the clip version selector.
class ClipVersion : public SecondaryContainer {
public:
    ClipVersion();

    void resized() override;

private:
    SelectableList versionList;
};
