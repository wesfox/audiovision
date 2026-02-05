#include "ClipSnapshot.h"

#include "Core/AudioClip/AudioClip.h"
#include "Utils/IO/AudioFile.h"

ClipSnapshot serializeClip(const AudioClip& clip, const Track& track) {
    ClipSnapshot snapshot;
    snapshot.trackId = track.getId();
    snapshot.clipId = clip.getId();
    snapshot.data.audioFilePath = clip.getAudioFilePath();
    snapshot.data.fileStartSample = clip.getFileStartSample();
    snapshot.data.sessionStartSample = clip.getSessionStartSample();
    snapshot.data.sessionEndSample = clip.getSessionEndSample();
    snapshot.data.sampleRate = clip.getSampleRate();
    snapshot.data.format = clip.getFormat();
    snapshot.data.muted = clip.isMuted();
    snapshot.data.name = clip.getName();
    snapshot.data.color = clip.getColor();
    snapshot.data.fadeInActive = clip.hasFadeIn();
    snapshot.data.fadeInEndSample = clip.getFadeInEndSample();
    snapshot.data.fadeOutActive = clip.hasFadeOut();
    snapshot.data.fadeOutStartSample = clip.getFadeOutStartSample();
    return snapshot;
}

std::unique_ptr<AudioClip> restoreClip(const ClipSnapshot& snapshot) {
    const auto filePath = snapshot.data.audioFilePath;
    if (filePath.isEmpty()) {
        // Audio file path must be valid to restore a clip.
        jassert(false);
        return {};
    }
    auto audioFile = AudioFile::get(juce::File(filePath));
    if (!audioFile) {
        // Audio file must be available to restore a clip.
        jassert(false);
        return {};
    }
    auto clip = AudioClip::create(audioFile,
                                  snapshot.data.fileStartSample,
                                  snapshot.data.sessionStartSample,
                                  snapshot.data.sessionEndSample);
    clip->setIdForRestore(snapshot.clipId);
    clip->setSampleRate(snapshot.data.sampleRate);
    clip->setFormat(snapshot.data.format);
    clip->mute(snapshot.data.muted);
    clip->setName(snapshot.data.name);
    clip->setColor(snapshot.data.color);
    clip->setFadeIn(snapshot.data.fadeInActive, snapshot.data.fadeInEndSample);
    clip->setFadeOut(snapshot.data.fadeOutActive, snapshot.data.fadeOutStartSample);
    return clip;
}
