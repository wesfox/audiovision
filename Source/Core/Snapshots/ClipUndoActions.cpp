#include "ClipUndoActions.h"

#include <unordered_map>
#include <unordered_set>

#include "Core/Edit/Edit.h"
#include "Core/Track/AudioTrack.h"

namespace {
std::vector<ClipSnapshot> snapshotTracks(const Edit& edit, const std::vector<String>& trackIds) {
    std::vector<ClipSnapshot> snapshots;
    std::unordered_set<String> trackIdSet;
    trackIdSet.reserve(trackIds.size());
    for (const auto& trackId : trackIds) {
        trackIdSet.insert(trackId);
    }

    for (const auto& track : edit.getTracks()) {
        if (!track || !track->isAudioTrack()) {
            continue;
        }
        const auto trackId = track->getId();
        if (trackIdSet.find(trackId) == trackIdSet.end()) {
            continue;
        }
        auto audioTrack = std::dynamic_pointer_cast<AudioTrack>(track);
        if (!audioTrack) {
            continue;
        }
        for (const auto& clip : audioTrack->getAudioClips()) {
            if (!clip) {
                continue;
            }
            snapshots.push_back(serializeClip(*clip, *track));
        }
    }
    return snapshots;
}

std::unordered_set<String> makeTrackIdSet(const std::vector<String>& trackIds) {
    std::unordered_set<String> trackIdSet;
    trackIdSet.reserve(trackIds.size());
    for (const auto& trackId : trackIds) {
        trackIdSet.insert(trackId);
    }
    return trackIdSet;
}

void restoreSnapshots(Edit& edit,
                      const std::vector<String>& trackIds,
                      const std::vector<ClipSnapshot>& snapshots) {
    std::unordered_map<String, std::vector<std::unique_ptr<AudioClip>>> clipsByTrack;
    clipsByTrack.reserve(trackIds.size());
    for (const auto& snapshot : snapshots) {
        auto clip = restoreClip(snapshot);
        if (!clip) {
            continue;
        }
        clipsByTrack[snapshot.trackId].push_back(std::move(clip));
    }

    const auto trackIdSet = makeTrackIdSet(trackIds);

    for (const auto& track : edit.getTracks()) {
        if (!track || !track->isAudioTrack()) {
            continue;
        }
        const auto trackId = track->getId();
        if (trackIdSet.find(trackId) == trackIdSet.end()) {
            continue;
        }
        auto audioTrack = std::dynamic_pointer_cast<AudioTrack>(track);
        if (!audioTrack) {
            continue;
        }
        auto clipsIt = clipsByTrack.find(trackId);
        if (clipsIt == clipsByTrack.end()) {
            audioTrack->replaceClips({});
            continue;
        }
        audioTrack->replaceClips(std::move(clipsIt->second));
    }
}
}

SplitClipsAction::SplitClipsAction(Edit& edit,
                                   std::vector<String> trackIds,
                                   std::vector<int64> splitSamples)
    : edit(edit),
      trackIds(std::move(trackIds)),
      splitSamples(std::move(splitSamples)) {
}

bool SplitClipsAction::perform() {
    if (!hasSnapshots) {
        beforeSnapshots = snapshotTracks(edit, trackIds);
        const auto trackIdSet = makeTrackIdSet(trackIds);
        for (const auto& track : edit.getTracks()) {
            if (!track || !track->isAudioTrack()) {
                continue;
            }
            const auto trackId = track->getId();
            if (trackIdSet.find(trackId) == trackIdSet.end()) {
                continue;
            }
            auto audioTrack = std::dynamic_pointer_cast<AudioTrack>(track);
            if (!audioTrack) {
                continue;
            }
            for (const auto splitSample : splitSamples) {
                audioTrack->splitClipsAtSample(splitSample);
            }
        }
        afterSnapshots = snapshotTracks(edit, trackIds);
        hasSnapshots = true;
        return true;
    }

    restoreSnapshots(edit, trackIds, afterSnapshots);
    return true;
}

bool SplitClipsAction::undo() {
    restoreSnapshots(edit, trackIds, beforeSnapshots);
    return true;
}

HealClipsAction::HealClipsAction(Edit& edit,
                                 std::vector<String> trackIds,
                                 int64 rangeStart,
                                 int64 rangeEnd)
    : edit(edit),
      trackIds(std::move(trackIds)),
      rangeStart(rangeStart),
      rangeEnd(rangeEnd) {
}

bool HealClipsAction::perform() {
    if (!hasSnapshots) {
        beforeSnapshots = snapshotTracks(edit, trackIds);
        const auto trackIdSet = makeTrackIdSet(trackIds);
        for (const auto& track : edit.getTracks()) {
            if (!track || !track->isAudioTrack()) {
                continue;
            }
            const auto trackId = track->getId();
            if (trackIdSet.find(trackId) == trackIdSet.end()) {
                continue;
            }
            auto audioTrack = std::dynamic_pointer_cast<AudioTrack>(track);
            if (!audioTrack) {
                continue;
            }
            audioTrack->healClipsInRange(rangeStart, rangeEnd);
        }
        afterSnapshots = snapshotTracks(edit, trackIds);
        hasSnapshots = true;
        return true;
    }

    restoreSnapshots(edit, trackIds, afterSnapshots);
    return true;
}

bool HealClipsAction::undo() {
    restoreSnapshots(edit, trackIds, beforeSnapshots);
    return true;
}

DeleteClipsAction::DeleteClipsAction(Edit& edit,
                                     std::vector<String> trackIds,
                                     int64 rangeStart,
                                     int64 rangeEnd)
    : edit(edit),
      trackIds(std::move(trackIds)),
      rangeStart(rangeStart),
      rangeEnd(rangeEnd) {
}

bool DeleteClipsAction::perform() {
    if (!hasSnapshots) {
        beforeSnapshots = snapshotTracks(edit, trackIds);
        const auto trackIdSet = makeTrackIdSet(trackIds);
        for (const auto& track : edit.getTracks()) {
            if (!track || !track->isAudioTrack()) {
                continue;
            }
            const auto trackId = track->getId();
            if (trackIdSet.find(trackId) == trackIdSet.end()) {
                continue;
            }
            auto audioTrack = std::dynamic_pointer_cast<AudioTrack>(track);
            if (!audioTrack) {
                continue;
            }
            audioTrack->deleteClipsInRange(rangeStart, rangeEnd);
        }
        afterSnapshots = snapshotTracks(edit, trackIds);
        hasSnapshots = true;
        return true;
    }

    restoreSnapshots(edit, trackIds, afterSnapshots);
    return true;
}

bool DeleteClipsAction::undo() {
    restoreSnapshots(edit, trackIds, beforeSnapshots);
    return true;
}

TrimToSelectionAction::TrimToSelectionAction(Edit& edit,
                                             std::vector<String> trackIds,
                                             int64 rangeStart,
                                             int64 rangeEnd)
    : edit(edit),
      trackIds(std::move(trackIds)),
      rangeStart(rangeStart),
      rangeEnd(rangeEnd) {
}

bool TrimToSelectionAction::perform() {
    if (!hasSnapshots) {
        beforeSnapshots = snapshotTracks(edit, trackIds);
        const auto trackIdSet = makeTrackIdSet(trackIds);
        for (const auto& track : edit.getTracks()) {
            if (!track || !track->isAudioTrack()) {
                continue;
            }
            const auto trackId = track->getId();
            if (trackIdSet.find(trackId) == trackIdSet.end()) {
                continue;
            }
            auto audioTrack = std::dynamic_pointer_cast<AudioTrack>(track);
            if (!audioTrack) {
                continue;
            }
            audioTrack->trimClipsToRange(rangeStart, rangeEnd);
        }
        afterSnapshots = snapshotTracks(edit, trackIds);
        hasSnapshots = true;
        return true;
    }

    restoreSnapshots(edit, trackIds, afterSnapshots);
    return true;
}

bool TrimToSelectionAction::undo() {
    restoreSnapshots(edit, trackIds, beforeSnapshots);
    return true;
}

namespace {
bool isCursorInsideClip(const AudioClip& clip, int64 cursorSample) {
    return cursorSample >= clip.getSessionStartSample()
        && cursorSample <= clip.getSessionEndSample();
}

bool isCursorInsideClipForSide(const AudioClip& clip,
                               int64 cursorSample,
                               TrimClipsToCursorAction::TrimSide side) {
    const auto clipStart = clip.getSessionStartSample();
    const auto clipEnd = clip.getSessionEndSample();
    if (clipEnd <= clipStart) {
        // Clip range must be valid to test cursor boundaries.
        jassert(false);
        return false;
    }
    if (side == TrimClipsToCursorAction::TrimSide::Tail) {
        return cursorSample >= (clipStart + 1) && cursorSample <= clipEnd;
    }
    return cursorSample >= clipStart && cursorSample <= (clipEnd - 1);
}
}

TrimClipsToCursorAction::TrimClipsToCursorAction(Edit& edit,
                                                 std::vector<String> trackIds,
                                                 int64 cursorSample,
                                                 TrimSide side)
    : edit(edit),
      trackIds(std::move(trackIds)),
      cursorSample(cursorSample),
      side(side) {
}

bool TrimClipsToCursorAction::perform() {
    if (!hasSnapshots) {
        beforeSnapshots = snapshotTracks(edit, trackIds);
        const auto trackIdSet = makeTrackIdSet(trackIds);
        for (const auto& track : edit.getTracks()) {
            if (!track || !track->isAudioTrack()) {
                continue;
            }
            const auto trackId = track->getId();
            if (trackIdSet.find(trackId) == trackIdSet.end()) {
                continue;
            }
            auto audioTrack = std::dynamic_pointer_cast<AudioTrack>(track);
            if (!audioTrack) {
                continue;
            }
            std::optional<String> targetClipId;
            std::optional<int64> targetClipStart;
            std::optional<int64> targetClipEnd;
            bool targetContainsCursor = false;
            for (const auto& clip : audioTrack->getAudioClips()) {
                if (clip && isCursorInsideClipForSide(*clip, cursorSample, side)) {
                    targetClipId = clip->getId();
                    targetClipStart = clip->getSessionStartSample();
                    targetClipEnd = clip->getSessionEndSample();
                    targetContainsCursor = true;
                    break;
                }
            }

            if (!targetClipId.has_value()) {
                std::optional<int64> bestSample;
                for (const auto& clip : audioTrack->getAudioClips()) {
                    if (!clip) {
                        continue;
                    }
                    if (side == TrimSide::Head) {
                        const auto clipStart = clip->getSessionStartSample();
                        if (clipStart >= cursorSample
                            && (!bestSample.has_value() || clipStart < bestSample.value())) {
                            bestSample = clipStart;
                            targetClipId = clip->getId();
                            targetClipStart = clipStart;
                            targetClipEnd = clip->getSessionEndSample();
                        }
                    } else {
                        const auto clipEnd = clip->getSessionEndSample();
                        if (clipEnd <= cursorSample
                            && (!bestSample.has_value() || clipEnd > bestSample.value())) {
                            bestSample = clipEnd;
                            targetClipId = clip->getId();
                            targetClipStart = clip->getSessionStartSample();
                            targetClipEnd = clipEnd;
                        }
                    }
                }
            }

            if (!targetClipId.has_value()) {
                continue;
            }

            if (!targetClipStart.has_value() || !targetClipEnd.has_value()) {
                continue;
            }

            if (side == TrimSide::Head) {
                audioTrack->trimClipHead(*targetClipId, cursorSample);
            } else {
                const auto targetSample = cursorSample;
                if (targetSample < targetClipStart.value()) {
                    // Tail trim target must not precede clip start.
                    jassert(false);
                    continue;
                }
                audioTrack->trimClipTail(*targetClipId, targetSample);
            }
        }
        afterSnapshots = snapshotTracks(edit, trackIds);
        hasSnapshots = true;
        return true;
    }

    restoreSnapshots(edit, trackIds, afterSnapshots);
    return true;
}

bool TrimClipsToCursorAction::undo() {
    restoreSnapshots(edit, trackIds, beforeSnapshots);
    return true;
}

namespace {
struct CutPair {
    String leftClipId;
    String rightClipId;
    int64 cutSample = 0;
};

std::optional<CutPair> findCutPair(const AudioTrack& track,
                                   int64 cursorSample,
                                   MoveCutToCursorAction::CutDirection direction) {
    std::vector<const AudioClip*> clips;
    clips.reserve(track.getAudioClips().size());
    for (const auto& clip : track.getAudioClips()) {
        if (clip) {
            clips.push_back(clip.get());
        }
    }
    std::sort(clips.begin(), clips.end(), [](const AudioClip* a, const AudioClip* b) {
        return a->getSessionStartSample() < b->getSessionStartSample();
    });

    std::optional<CutPair> best;
    int containingIndex = -1;
    for (size_t index = 0; index < clips.size(); ++index) {
        const auto* clip = clips[index];
        if (!clip) {
            continue;
        }
        if (cursorSample >= clip->getSessionStartSample()
            && cursorSample <= clip->getSessionEndSample()) {
            containingIndex = static_cast<int>(index);
            break;
        }
    }

    if (containingIndex >= 0) {
        if (direction == MoveCutToCursorAction::CutDirection::Next) {
            if (containingIndex - 1 >= 0) {
                const auto* left = clips[static_cast<size_t>(containingIndex - 1)];
                const auto* right = clips[static_cast<size_t>(containingIndex)];
                if (left && right) {
                    best = CutPair { left->getId(), right->getId(), right->getSessionStartSample() };
                }
            }
        } else {
            if (containingIndex + 1 < static_cast<int>(clips.size())) {
                const auto* left = clips[static_cast<size_t>(containingIndex)];
                const auto* right = clips[static_cast<size_t>(containingIndex + 1)];
                if (left && right) {
                    best = CutPair { left->getId(), right->getId(), right->getSessionStartSample() };
                }
            }
        }
        return best;
    }
    for (size_t index = 1; index < clips.size(); ++index) {
        const auto* left = clips[index - 1];
        const auto* right = clips[index];
        if (!left || !right) {
            continue;
        }
        const auto cutSample = right->getSessionStartSample();
        if (direction == MoveCutToCursorAction::CutDirection::Next) {
            if (cutSample > cursorSample
                && (!best.has_value() || cutSample < best->cutSample)) {
                best = CutPair { left->getId(), right->getId(), cutSample };
            }
        } else {
            if (cutSample < cursorSample
                && (!best.has_value() || cutSample > best->cutSample)) {
                best = CutPair { left->getId(), right->getId(), cutSample };
            }
        }
    }
    return best;
}
} // namespace

MoveCutToCursorAction::MoveCutToCursorAction(Edit& edit,
                                             std::vector<String> trackIds,
                                             int64 cursorSample,
                                             CutDirection direction)
    : edit(edit),
      trackIds(std::move(trackIds)),
      cursorSample(cursorSample),
      direction(direction) {
}

bool MoveCutToCursorAction::perform() {
    if (!hasSnapshots) {
        beforeSnapshots = snapshotTracks(edit, trackIds);
        const auto trackIdSet = makeTrackIdSet(trackIds);
        for (const auto& track : edit.getTracks()) {
            if (!track || !track->isAudioTrack()) {
                continue;
            }
            const auto trackId = track->getId();
            if (trackIdSet.find(trackId) == trackIdSet.end()) {
                continue;
            }
            auto audioTrack = std::dynamic_pointer_cast<AudioTrack>(track);
            if (!audioTrack) {
                continue;
            }
            const auto cutPair = findCutPair(*audioTrack, cursorSample, direction);
            if (!cutPair.has_value()) {
                continue;
            }
            const auto tailSample = cursorSample - 1;
            if (tailSample < 0) {
                // Cursor sample must be positive to move cuts.
                jassert(false);
                continue;
            }
            audioTrack->trimClipTail(cutPair->leftClipId, tailSample);
            audioTrack->trimClipHead(cutPair->rightClipId, cursorSample);
        }
        afterSnapshots = snapshotTracks(edit, trackIds);
        hasSnapshots = true;
        return true;
    }

    restoreSnapshots(edit, trackIds, afterSnapshots);
    return true;
}

bool MoveCutToCursorAction::undo() {
    restoreSnapshots(edit, trackIds, beforeSnapshots);
    return true;
}
