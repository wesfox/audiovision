#pragma once

#include <JuceHeader.h>

#include "Gui/Components/Common/PrimaryContainer.h"
#include "ContextualComponent/InsertsComponent.h"
#include "ContextualComponent/SendsComponent.h"

/// Displays the inspector section.
class InspectorSection : public PrimaryContainer {
public:
    InspectorSection();

    void resized() override;

private:
    InsertsComponent insertsComponent;
    SendsComponent sendsComponent;
};
