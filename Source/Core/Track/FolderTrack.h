#pragma once

#include <JuceHeader.h>
#include "Track.h"

/// Folder track used to group child tracks.
class FolderTrack : public Track {
public:
    /// Create a folder track with an optional parent.
    /// @param parentFolder parent folder (nullable)
    /// @param name display name
    FolderTrack(FolderTrack* parentFolder, const String& name="");

    /// Create a shared folder track.
    /// @param parentFolder parent folder (nullable)
    /// @param name display name
    static std::shared_ptr<FolderTrack> create(FolderTrack* parentFolder = nullptr, const String& name="")
    {
        return std::make_shared<FolderTrack>(parentFolder, name);
    }

    /// Add a track to this folder.
    /// @param track track to add
    void addTrack(std::weak_ptr<Track> track) {
        tracks.push_back(std::move(track));
    }

    /// Attach a child folder to this folder.
    /// @param childFolderToAttach child folder to attach
    void attachChildFolder(std::shared_ptr<FolderTrack> childFolderToAttach) {
        childFolderToAttach.get()->setParentFolder(this);
        childrenFolder.push_back(std::move(childFolderToAttach));
    }

private:
    /// Assign the parent folder (internal use).
    /// @param newParentFolder new parent folder
    void setParentFolder(FolderTrack* newParentFolder) {
        parentFolder = newParentFolder;
    }

    std::vector<std::weak_ptr<Track>> tracks;
    FolderTrack* parentFolder;
    std::vector<std::shared_ptr<FolderTrack>> childrenFolder;
};
