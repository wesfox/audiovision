#pragma once

#include <JuceHeader.h>

#include "../Common/ui/RectToggleButton.h"
#include "../Common/Containers/SecondaryContainer.h"

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
