#include "TrackCommandManager.h"

#include "Core/Track/FolderTrack.h"

#include <algorithm>
#include <unordered_map>
#include <unordered_set>

TrackCommandManager::TrackCommandManager(Edit& edit)
    : edit(edit) {
}

namespace {
TrackArmState getNextState(TrackArmState state) {
    return state == TrackArmState::Active ? TrackArmState::Inactive : TrackArmState::Active;
}

TrackInputMonitoringState getNextInputMonitoringState(TrackInputMonitoringState state) {
    return state == TrackInputMonitoringState::Active ? TrackInputMonitoringState::Inactive : TrackInputMonitoringState::Active;
}

TrackSoloState getNextState(TrackSoloState state) {
    return state == TrackSoloState::Solo ? TrackSoloState::Inactive : TrackSoloState::Solo;
}

TrackMuteState getNextState(TrackMuteState state) {
    return state == TrackMuteState::Mute ? TrackMuteState::Active : TrackMuteState::Mute;
}
}

void TrackCommandManager::setArmState(const String& trackId, TrackArmState state) {
    edit.getState().setTrackArmState(trackId, state, &edit.getUndoManager());
}

void TrackCommandManager::toggleArmState(const String& trackId) {
    const auto current = edit.getState().getTrackArmState(trackId);
    setArmState(trackId, getNextState(current));
}

void TrackCommandManager::setInputMonitoringState(const String& trackId, TrackInputMonitoringState state) {
    edit.getState().setTrackInputMonitoringState(trackId, state, &edit.getUndoManager());
}

void TrackCommandManager::toggleInputMonitoringState(const String& trackId) {
    const auto current = edit.getState().getTrackInputMonitoringState(trackId);
    setInputMonitoringState(trackId, getNextInputMonitoringState(current));
}

void TrackCommandManager::setSoloState(const String& trackId, TrackSoloState state) {
    setSoloStateInternal(trackId, state);
    updateMuteStates();
}

void TrackCommandManager::requestSoloState(const String& trackId, TrackSoloState requestedState) {
    auto track = getTrackById(trackId);
    if (!track) {
        return;
    }

    if (requestedState == TrackSoloState::Solo) {
        if (track->getTrackType() == TrackType::Folder) {
            applySoloOnFolder(std::dynamic_pointer_cast<FolderTrack>(track), true);
        } else {
            applySoloOnAudio(track, true);
        }
    } else {
        if (track->getTrackType() == TrackType::Folder) {
            applySoloOnFolder(std::dynamic_pointer_cast<FolderTrack>(track), false);
        } else {
            applySoloOnAudio(track, false);
        }
    }

    updateMuteStates();
}

void TrackCommandManager::toggleSoloState(const String& trackId) {
    auto track = getTrackById(trackId);
    if (!track) {
        return;
    }

    const auto currentState = edit.getState().getTrackSoloState(trackId);
    const bool isCurrentlySolo = currentState == TrackSoloState::Solo;
    const auto soloMode = edit.getState().getSoloMode();

    if (soloMode == EditState::SoloMode::Xor && !isCurrentlySolo) {
        clearAllSoloStates();
        if (track->getTrackType() == TrackType::Folder) {
            applySoloOnFolder(std::dynamic_pointer_cast<FolderTrack>(track), true);
        } else {
            applySoloOnAudio(track, true);
        }
        updateMuteStates();
        return;
    }

    if (isCurrentlySolo) {
        if (track->getTrackType() == TrackType::Folder) {
            applySoloOnFolder(std::dynamic_pointer_cast<FolderTrack>(track), false);
        } else {
            applySoloOnAudio(track, false);
        }
    } else {
        if (track->getTrackType() == TrackType::Folder) {
            applySoloOnFolder(std::dynamic_pointer_cast<FolderTrack>(track), true);
        } else {
            applySoloOnAudio(track, true);
        }
    }
    updateMuteStates();
}

void TrackCommandManager::toggleSoloSafeState(const String& trackId) {
    const auto currentSafe = edit.getState().getTrackSoloSafeState(trackId);
    const auto nextSafe = !currentSafe;
    edit.getState().setTrackSoloSafeState(trackId, nextSafe, &edit.getUndoManager());

    const auto soloState = edit.getState().getTrackSoloState(trackId);
    if (nextSafe) {
        if (soloState == TrackSoloState::Inactive) {
            setSoloStateInternal(trackId, TrackSoloState::SoloSafe);
        }
    } else {
        if (soloState == TrackSoloState::SoloSafe) {
            setSoloStateInternal(trackId, TrackSoloState::Inactive);
        }
    }
    updateMuteStates();
}

void TrackCommandManager::clearAndSolo(const String& trackId) {
    auto track = getTrackById(trackId);
    if (!track) {
        return;
    }

    if (edit.getState().getTrackSoloState(trackId) == TrackSoloState::Solo) {
        clearAllSoloStates();
        updateMuteStates();
        return;
    }

    clearAllSoloStates();

    if (track->getTrackType() == TrackType::Folder) {
        applySoloOnFolder(std::dynamic_pointer_cast<FolderTrack>(track), true);
    } else {
        applySoloOnAudio(track, true);
    }
    updateMuteStates();
}

void TrackCommandManager::toggleSoloSelection(const std::vector<String>& trackIds) {
    if (trackIds.empty()) {
        return;
    }

    std::unordered_map<String, int> orderById;
    orderById.reserve(edit.getTracks().size());
    int orderIndex = 0;
    for (const auto& track : edit.getTracks()) {
        if (!track) {
            continue;
        }
        orderById.emplace(track->getId(), orderIndex++);
    }

    struct SelectionEntry {
        std::shared_ptr<Track> track;
        int depth = 0;
        int order = 0;
    };

    std::vector<SelectionEntry> selection;
    selection.reserve(trackIds.size());
    std::unordered_set<String> seenIds;
    for (const auto& trackId : trackIds) {
        if (!seenIds.insert(trackId).second) {
            continue;
        }
        auto track = getTrackById(trackId);
        if (!track) {
            continue;
        }
        int depth = 0;
        for (auto* parentFolder = track->getParentFolder(); parentFolder != nullptr; parentFolder = parentFolder->getParentFolder()) {
            ++depth;
        }
        const auto orderIt = orderById.find(trackId);
        const int order = orderIt != orderById.end() ? orderIt->second : depth;
        selection.push_back({ track, depth, order });
    }

    if (selection.empty()) {
        return;
    }

    std::sort(selection.begin(), selection.end(), [](const SelectionEntry& lhs, const SelectionEntry& rhs) {
        if (lhs.depth != rhs.depth) {
            return lhs.depth < rhs.depth;
        }
        return lhs.order < rhs.order;
    });

    const auto soloMode = edit.getState().getSoloMode();
    if (soloMode == EditState::SoloMode::Xor) {
        clearAllSoloStates();
        for (const auto& entry : selection) {
            if (entry.track->getTrackType() == TrackType::Folder) {
                applySoloOnFolder(std::dynamic_pointer_cast<FolderTrack>(entry.track), true);
            } else {
                applySoloOnAudio(entry.track, true);
            }
        }
        updateMuteStates();
        return;
    }

    for (const auto& entry : selection) {
        const auto trackId = entry.track->getId();
        const auto currentState = edit.getState().getTrackSoloState(trackId);
        const bool enable = currentState != TrackSoloState::Solo;
        if (entry.track->getTrackType() == TrackType::Folder) {
            applySoloOnFolder(std::dynamic_pointer_cast<FolderTrack>(entry.track), enable);
        } else {
            applySoloOnAudio(entry.track, enable);
        }
    }
    updateMuteStates();
}

void TrackCommandManager::toggleMuteSelection(const std::vector<String>& trackIds) {
    if (trackIds.empty()) {
        return;
    }

    bool didChange = false;
    for (const auto& trackId : trackIds) {
        const auto current = edit.getState().getTrackUserMuteState(trackId);
        const auto next = getNextState(current);
        if (next != current) {
            edit.getState().setTrackUserMuteState(trackId, next, &edit.getUndoManager());
            didChange = true;
        }
    }

    if (didChange) {
        updateMuteStates();
    }
}

void TrackCommandManager::clearAllSolo() {
    clearAllSoloStates();
    updateMuteStates();
}

void TrackCommandManager::setMuteState(const String& trackId, TrackMuteState state) {
    edit.getState().setTrackMuteState(trackId, state, &edit.getUndoManager());
}

void TrackCommandManager::setUserMuteState(const String& trackId, TrackMuteState state) {
    edit.getState().setTrackUserMuteState(trackId, state, &edit.getUndoManager());
    updateMuteStates();
}

void TrackCommandManager::toggleMuteState(const String& trackId) {
    const auto current = edit.getState().getTrackUserMuteState(trackId);
    setUserMuteState(trackId, getNextState(current));
}

void TrackCommandManager::applySoloOnAudio(const std::shared_ptr<Track>& track, bool enable) {
    if (!track) {
        return;
    }

    if (enable) {
        setSoloStateInternal(track->getId(), TrackSoloState::Solo);
        for (auto* parentFolder = track->getParentFolder(); parentFolder != nullptr; parentFolder = parentFolder->getParentFolder()) {
            setSoloStateInternal(parentFolder->getId(), TrackSoloState::Solo);
        }
        return;
    }

    setSoloStateInternal(track->getId(), getDisabledSoloState(track->getId()));
    auto* parentFolder = track->getParentFolder();
    while (parentFolder != nullptr) {
        if (hasActiveSoloInFolder(*parentFolder)) {
            break;
        }
        setSoloStateInternal(parentFolder->getId(), getDisabledSoloState(parentFolder->getId()));
        parentFolder = parentFolder->getParentFolder();
    }
}

void TrackCommandManager::applySoloOnFolder(const std::shared_ptr<FolderTrack>& folder, bool enable) {
    if (!folder) {
        return;
    }

    if (enable) {
        setFolderAndDescendantsSolo(*folder, TrackSoloState::Solo);
        for (auto* parentFolder = folder->getParentFolder(); parentFolder != nullptr; parentFolder = parentFolder->getParentFolder()) {
            setSoloStateInternal(parentFolder->getId(), TrackSoloState::Solo);
        }
        return;
    }

    setFolderAndDescendantsSoloDisabled(*folder);
    auto* parentFolder = folder->getParentFolder();
    while (parentFolder != nullptr) {
        if (hasActiveSoloInFolder(*parentFolder)) {
            break;
        }
        setSoloStateInternal(parentFolder->getId(), getDisabledSoloState(parentFolder->getId()));
        parentFolder = parentFolder->getParentFolder();
    }
}

bool TrackCommandManager::hasActiveSoloInFolder(const FolderTrack& folder) const {
    const auto& state = edit.getState();
    for (const auto& childTrackRef : folder.getChildTracks()) {
        if (auto childTrack = childTrackRef.lock()) {
            if (state.getTrackSoloState(childTrack->getId()) == TrackSoloState::Solo) {
                return true;
            }
        }
    }

    for (const auto& childFolder : folder.getChildFolders()) {
        if (!childFolder) {
            continue;
        }
        if (state.getTrackSoloState(childFolder->getId()) == TrackSoloState::Solo) {
            return true;
        }
        if (hasActiveSoloInFolder(*childFolder)) {
            return true;
        }
    }

    return false;
}

void TrackCommandManager::setFolderAndDescendantsSolo(FolderTrack& folder, TrackSoloState state) {
    setSoloStateInternal(folder.getId(), state);
    for (const auto& childTrackRef : folder.getChildTracks()) {
        if (auto childTrack = childTrackRef.lock()) {
            setSoloStateInternal(childTrack->getId(), state);
        }
    }
    for (const auto& childFolder : folder.getChildFolders()) {
        if (childFolder) {
            setFolderAndDescendantsSolo(*childFolder, state);
        }
    }
}

void TrackCommandManager::setFolderAndDescendantsSoloDisabled(FolderTrack& folder) {
    setSoloStateInternal(folder.getId(), getDisabledSoloState(folder.getId()));
    for (const auto& childTrackRef : folder.getChildTracks()) {
        if (auto childTrack = childTrackRef.lock()) {
            setSoloStateInternal(childTrack->getId(), getDisabledSoloState(childTrack->getId()));
        }
    }
    for (const auto& childFolder : folder.getChildFolders()) {
        if (childFolder) {
            setFolderAndDescendantsSoloDisabled(*childFolder);
        }
    }
}

TrackSoloState TrackCommandManager::getDisabledSoloState(const String& trackId) const {
    return edit.getState().getTrackSoloSafeState(trackId) ? TrackSoloState::SoloSafe
                                                          : TrackSoloState::Inactive;
}

void TrackCommandManager::setSoloStateInternal(const String& trackId, TrackSoloState state) {
    edit.getState().setTrackSoloState(trackId, state, &edit.getUndoManager());
}

std::shared_ptr<Track> TrackCommandManager::getTrackById(const String& trackId) const {
    for (const auto& track : edit.getTracks()) {
        if (track && track->getId() == trackId) {
            return track;
        }
    }
    return {};
}

void TrackCommandManager::clearAllSoloStates() {
    for (const auto& track : edit.getTracks()) {
        if (!track) {
            continue;
        }
        setSoloStateInternal(track->getId(), getDisabledSoloState(track->getId()));
    }
}

void TrackCommandManager::updateMuteStates() {
    const auto& tracks = edit.getTracks();
    bool anySolo = false;
    for (const auto& track : tracks) {
        if (!track) {
            continue;
        }
        const auto soloState = edit.getState().getTrackSoloState(track->getId());
        if (soloState == TrackSoloState::Solo) {
            anySolo = true;
            break;
        }
    }

    for (const auto& track : tracks) {
        if (!track) {
            continue;
        }
        const auto trackId = track->getId();
        const auto soloState = edit.getState().getTrackSoloState(trackId);
        const auto userMuteState = edit.getState().getTrackUserMuteState(trackId);
        const bool isSolo = soloState == TrackSoloState::Solo;
        const bool isSoloSafe = soloState == TrackSoloState::SoloSafe;
        const bool impliedMute = anySolo && !isSolo && !isSoloSafe;
        TrackMuteState effectiveState = TrackMuteState::Active;
        if (userMuteState == TrackMuteState::Mute) {
            effectiveState = TrackMuteState::Mute;
        } else if (impliedMute) {
            effectiveState = TrackMuteState::SoloMute;
        }
        setMuteState(trackId, effectiveState);
    }
}
