#include "GainLine.h"

#include "spdlog/common.h"

// ------------------------ MainComponent Implementation ------------------------

GainLine::GainLine() {
}

std::unique_ptr<GainLine> GainLine::split(int64 splitSample) {
    auto newGainLine = std::make_unique<GainLine>();
    GainLine* newGainLineRef = newGainLine.get();
    return newGainLine;
}
