#pragma once

#include <JuceHeader.h>

struct RecordState {
    bool isRecording = false;
    int64 startSample = 0;
    int64 endSample = 0;
};
