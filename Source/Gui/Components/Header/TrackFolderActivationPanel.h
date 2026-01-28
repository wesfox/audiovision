#pragma once

#include <JuceHeader.h>

#include "Gui/Components/Common/RectToggleButton.h"
#include "Gui/Components/Common/SecondaryContainer.h"

/// Displays track folder activation toggles.
class TrackFolderActivationPanel : public SecondaryContainer {
public:
    explicit TrackFolderActivationPanel(const Edit* edit = nullptr);

    void resized() override;

    static constexpr int kWidth = 166;

private:
    RectToggleButton dial;
    RectToggleButton foley;
    RectToggleButton amb;
    RectToggleButton fx;
    RectToggleButton mus;
};
