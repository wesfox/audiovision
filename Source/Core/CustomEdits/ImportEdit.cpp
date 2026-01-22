#include "ImportEdit.h"

#include "Utils/IO/EditSerializer.h"

// ------------------------ MainComponent Implementation ------------------------

ImportEdit::ImportEdit() {
}

std::shared_ptr<Edit> ImportEdit::importEdit() {
    auto fileToImport = juce::File("/Users/nico/code/Audiovision/exports/audiovision.json");
    return EditSerializer::importFromFile(fileToImport);
}
