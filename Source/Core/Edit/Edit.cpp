#include "Edit.h"

#include <algorithm>
#include <cmath>

#include "Core/Track/AudioTrack.h"

Edit::Edit()
    : transport(std::make_shared<Transport>()),
      projectName(""),
      automationManager(),
      videoStartFrame(0),
      video(),
      videoSyncOffset(0),
      editState(),
      undoManager(),
      actionStore(editState, undoManager) {
    clampViewToSessionClips();
}

int64 Edit::getSessionEndSample() const {
    int64 maxEndSample = 0;
    for (const auto& track : tracks) {
        auto audioTrack = std::dynamic_pointer_cast<AudioTrack>(track);
        if (!audioTrack) {
            continue;
        }
        for (const auto& clip : audioTrack->getAudioClips()) {
            if (!clip) {
                continue;
            }
            maxEndSample = std::max(maxEndSample, clip->getSessionEndSample());
        }
    }
    return maxEndSample;
}

void Edit::clampViewToSessionClips() {
    const auto viewWidth = static_cast<double>(editState.getViewWidthPixels());
    if (viewWidth <= 0.0) {
        // View width must be positive to fit the session view.
        jassert(false);
        return;
    }

    const auto sessionEndSample = getSessionEndSample();
    const auto fallbackSamplesPerPixel = 4096.0;
    const auto targetSamplesPerPixel = sessionEndSample > 0
        ? static_cast<double>(sessionEndSample) / viewWidth
        : fallbackSamplesPerPixel;
    const auto clampedTarget = std::max(targetSamplesPerPixel, 1e-6);
    const auto exponent = std::ceil(std::log2(clampedTarget));
    const auto snappedSamplesPerPixel = std::pow(2.0, exponent);

    editState.setViewStartSampleF(0.0, nullptr);
    editState.setSamplesPerPixel(snappedSamplesPerPixel, nullptr);
}
