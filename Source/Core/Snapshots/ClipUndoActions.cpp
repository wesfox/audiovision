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
