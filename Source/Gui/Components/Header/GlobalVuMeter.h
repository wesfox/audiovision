#pragma once

#include <JuceHeader.h>

#include "../Common/Containers/SecondaryContainer.h"
#include "../Common/tools/VuMeter.h"

/// Displays the global VU meters inside a secondary container.
class GlobalVuMeter : public SecondaryContainer {
public:
    explicit GlobalVuMeter(const Edit* edit = nullptr);

    void resized() override;

private:
    VuMeter vuMeter1;
    VuMeter vuMeter2;
    VuMeter vuMeter3;
    VuMeter vuMeter4;
    VuMeter vuMeter5;
    VuMeter vuMeter6;
};
