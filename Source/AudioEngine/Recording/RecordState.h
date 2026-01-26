#pragma once

#include <JuceHeader.h>

/// Simple recording state snapshot used by RecordSession.
struct RecordState {
    /// True while a record pass is active.
    bool isRecording = false;
    /// Absolute timeline sample where recording started.
    int64 startSample = 0;
    /// Absolute timeline sample where recording ended.
    int64 endSample = 0;
};
