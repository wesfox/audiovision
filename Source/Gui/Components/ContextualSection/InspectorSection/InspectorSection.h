#pragma once

#include <JuceHeader.h>

#include "../../Common/Containers/PrimaryContainer.h"
#include "InsertsComponent.h"
#include "SendsComponent.h"
#include "VolumeAndPan.h"
#include "ClipVersion.h"

/// Displays the inspector section.
class InspectorSection : public PrimaryContainer {
public:
    InspectorSection();

    void resized() override;

private:
    InsertsComponent insertsComponent;
    SendsComponent sendsComponent;
    VolumeAndPan volumeAndPan;
    ClipVersion clipVersion;
};
