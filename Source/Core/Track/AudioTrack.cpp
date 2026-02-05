#include "AudioTrack.h"

AudioTrack::AudioTrack(const String &name)
    : Track(name)
      //recorder(Recorder())
{
    isAudioTrack_ = true;
    trackType = TrackType::Audio;
}

void AudioTrack::replaceClips(std::vector<std::unique_ptr<AudioClip>> clips) {
    audioClips = std::move(clips);
    notifyClipsChanged();
}

void AudioTrack::splitClipsAtSample(int64 splitSample) {
    if (splitSample < 0) {
        // Split sample must be non-negative.
        jassert(false);
        return;
    }

    using ClipContainer = std::vector<std::unique_ptr<AudioClip>>;
    bool didSplit = false;
    for (size_t index = 0; index < audioClips.size(); ++index) {
        auto& clip = audioClips[index];
        if (!clip) {
            continue;
        }
        const auto clipStart = clip->getSessionStartSample();
        const auto clipEnd = clip->getSessionEndSample();
        if (splitSample <= clipStart || splitSample >= clipEnd) {
            continue;
        }
        auto newClip = clip->split(splitSample);
        if (!newClip) {
            continue;
        }
        const auto insertIndex = static_cast<ClipContainer::difference_type>(index + 1);
        audioClips.insert(audioClips.begin() + insertIndex, std::move(newClip));
        didSplit = true;
        ++index;
    }

    if (didSplit) {
        notifyClipsChanged();
    }
}

void AudioTrack::healClipsInRange(int64 rangeStart, int64 rangeEnd) {
    if (rangeStart < 0 || rangeEnd < 0) {
        // Range samples must be non-negative.
        jassert(false);
        return;
    }
    if (rangeStart > rangeEnd) {
        // Range start must be <= range end.
        jassert(false);
        return;
    }

    using ClipContainer = std::vector<std::unique_ptr<AudioClip>>;
    bool didHeal = false;
    for (size_t index = 0; index + 1 < audioClips.size();) {
        auto& leftClip = audioClips[index];
        auto& rightClip = audioClips[index + 1];
        if (!leftClip || !rightClip) {
            ++index;
            continue;
        }

        const auto leftEnd = leftClip->getSessionEndSample();
        const auto rightStart = rightClip->getSessionStartSample();
        const bool leftEndInRange = leftEnd >= rangeStart && leftEnd <= rangeEnd;
        const bool rightStartInRange = rightStart >= rangeStart && rightStart <= rangeEnd;
        if (!leftEndInRange && !rightStartInRange) {
            ++index;
            continue;
        }
        if (leftClip->getAudioFile() != rightClip->getAudioFile()) {
            ++index;
            continue;
        }
        if (leftClip->hasFadeOut() || rightClip->hasFadeIn()) {
            ++index;
            continue;
        }
        const auto sessionDelta = rightStart - leftClip->getSessionStartSample();
        const auto fileDelta = rightClip->getFileStartSample() - leftClip->getFileStartSample();
        if (sessionDelta != fileDelta) {
            ++index;
            continue;
        }

        leftClip->setSessionEndSample(rightClip->getSessionEndSample());
        audioClips.erase(audioClips.begin() + static_cast<ClipContainer::difference_type>(index + 1));
        didHeal = true;
        continue;
    }

    if (didHeal) {
        notifyClipsChanged();
    }
}

void AudioTrack::deleteClipsInRange(int64 rangeStart, int64 rangeEnd) {
    if (rangeStart < 0 || rangeEnd < 0) {
        // Range samples must be non-negative.
        jassert(false);
        return;
    }
    if (rangeStart > rangeEnd) {
        // Range start must be <= range end.
        jassert(false);
        return;
    }

    using ClipContainer = std::vector<std::unique_ptr<AudioClip>>;
    bool didChange = false;
    auto splitWithoutNotify = [this, &didChange](int64 splitSample) {
        if (splitSample < 0) {
            // Split sample must be non-negative.
            jassert(false);
            return;
        }
        for (size_t index = 0; index < audioClips.size(); ++index) {
            auto& clip = audioClips[index];
            if (!clip) {
                continue;
            }
            const auto clipStart = clip->getSessionStartSample();
            const auto clipEnd = clip->getSessionEndSample();
            if (splitSample <= clipStart || splitSample >= clipEnd) {
                continue;
            }
            auto newClip = clip->split(splitSample);
            if (!newClip) {
                continue;
            }
            const auto insertIndex = static_cast<ClipContainer::difference_type>(index + 1);
            audioClips.insert(audioClips.begin() + insertIndex, std::move(newClip));
            didChange = true;
            ++index;
        }
    };

    splitWithoutNotify(rangeStart);
    if (rangeEnd != rangeStart) {
        splitWithoutNotify(rangeEnd);
    }

    for (auto it = audioClips.begin(); it != audioClips.end();) {
        const auto& clip = *it;
        if (!clip) {
            ++it;
            continue;
        }
        const auto clipStart = clip->getSessionStartSample();
        const auto clipEnd = clip->getSessionEndSample();
        if (clipStart >= rangeStart && clipEnd <= rangeEnd) {
            it = audioClips.erase(it);
            didChange = true;
            continue;
        }
        ++it;
    }

    if (didChange) {
        notifyClipsChanged();
    }
}

void AudioTrack::trimClipsToRange(int64 rangeStart, int64 rangeEnd) {
    if (rangeStart < 0 || rangeEnd < 0) {
        // Range samples must be non-negative.
        jassert(false);
        return;
    }
    if (rangeStart > rangeEnd) {
        // Range start must be <= range end.
        jassert(false);
        return;
    }

    using ClipContainer = std::vector<std::unique_ptr<AudioClip>>;
    bool didChange = false;
    auto splitWithoutNotify = [this, &didChange](int64 splitSample) {
        if (splitSample < 0) {
            // Split sample must be non-negative.
            jassert(false);
            return;
        }
        for (size_t index = 0; index < audioClips.size(); ++index) {
            auto& clip = audioClips[index];
            if (!clip) {
                continue;
            }
            const auto clipStart = clip->getSessionStartSample();
            const auto clipEnd = clip->getSessionEndSample();
            if (splitSample <= clipStart || splitSample >= clipEnd) {
                continue;
            }
            auto newClip = clip->split(splitSample);
            if (!newClip) {
                continue;
            }
            const auto insertIndex = static_cast<ClipContainer::difference_type>(index + 1);
            audioClips.insert(audioClips.begin() + insertIndex, std::move(newClip));
            didChange = true;
            ++index;
        }
    };

    splitWithoutNotify(rangeStart);
    if (rangeEnd != rangeStart) {
        splitWithoutNotify(rangeEnd);
    }

    for (auto it = audioClips.begin(); it != audioClips.end();) {
        const auto& clip = *it;
        if (!clip) {
            ++it;
            continue;
        }
        const auto clipStart = clip->getSessionStartSample();
        const auto clipEnd = clip->getSessionEndSample();
        if (clipEnd <= rangeStart || clipStart >= rangeEnd) {
            it = audioClips.erase(it);
            didChange = true;
            continue;
        }
        ++it;
    }

    if (didChange) {
        notifyClipsChanged();
    }
}

namespace {
bool hasOverlap(int64 startA, int64 endA, int64 startB, int64 endB) {
    return startA < endB && endA > startB;
}

std::vector<std::unique_ptr<AudioClip>> resolveOverlapWithClip(std::vector<std::unique_ptr<AudioClip>> clips,
                                                                const String& targetId,
                                                                int64 targetStart,
                                                                int64 targetEnd) {
    std::vector<std::unique_ptr<AudioClip>> next;
    next.reserve(clips.size() + 1);

    for (auto& clip : clips) {
        if (!clip) {
            continue;
        }
        if (clip->getId() == targetId) {
            next.push_back(std::move(clip));
            continue;
        }
        const auto clipStart = clip->getSessionStartSample();
        const auto clipEnd = clip->getSessionEndSample();
        if (!hasOverlap(clipStart, clipEnd, targetStart, targetEnd)) {
            next.push_back(std::move(clip));
            continue;
        }

        if (clipStart < targetStart && clipEnd > targetEnd) {
            auto right = clip->split(targetStart);
            next.push_back(std::move(clip));
            if (right) {
                auto tail = right->split(targetEnd);
                if (tail) {
                    next.push_back(std::move(tail));
                }
            }
            continue;
        }

        if (clipStart < targetStart && clipEnd > targetStart) {
            clip->trimTail(targetStart, false);
            next.push_back(std::move(clip));
            continue;
        }

        if (clipStart < targetEnd && clipEnd > targetEnd) {
            clip->trimHead(targetEnd, false);
            next.push_back(std::move(clip));
            continue;
        }
        // Fully covered by target; drop.
    }

    return next;
}
} // namespace

void AudioTrack::trimClipHead(const String& clipId, int64 newStartSample) {
    for (const auto& clip : audioClips) {
        if (clip && clip->getId() == clipId) {
            clip->trimHead(newStartSample, false);
            const auto targetStart = clip->getSessionStartSample();
            const auto targetEnd = clip->getSessionEndSample();
            audioClips = resolveOverlapWithClip(std::move(audioClips), clipId, targetStart, targetEnd);
            notifyClipsChanged();
            return;
        }
    }
}

void AudioTrack::trimClipTail(const String& clipId, int64 newEndSample) {
    for (const auto& clip : audioClips) {
        if (clip && clip->getId() == clipId) {
            clip->trimTail(newEndSample, false);
            const auto targetStart = clip->getSessionStartSample();
            const auto targetEnd = clip->getSessionEndSample();
            audioClips = resolveOverlapWithClip(std::move(audioClips), clipId, targetStart, targetEnd);
            notifyClipsChanged();
            return;
        }
    }
}

void AudioTrack::previewTrimClipHead(const String& clipId, int64 newStartSample) {
    for (const auto& clip : audioClips) {
        if (clip && clip->getId() == clipId) {
            clip->trimHead(newStartSample, false);
            notifyClipsChanged();
            return;
        }
    }
}

void AudioTrack::previewTrimClipTail(const String& clipId, int64 newEndSample) {
    for (const auto& clip : audioClips) {
        if (clip && clip->getId() == clipId) {
            clip->trimTail(newEndSample, false);
            notifyClipsChanged();
            return;
        }
    }
}
