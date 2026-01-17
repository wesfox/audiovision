#pragma once

#include <JuceHeader.h>
#include "Track.h"

class FolderTrack : public Track {
public:
    FolderTrack(FolderTrack* parentFolder);

    /// This function returns a new FolderTrack
    static std::shared_ptr<FolderTrack> create(FolderTrack* parentFolder = nullptr)
    {
        return std::make_shared<FolderTrack>(parentFolder);
    }

    void addTrack(std::weak_ptr<Track> track) {
        tracks.push_back(std::move(track));
    }

    void attachChildFolder(std::shared_ptr<FolderTrack> childFolderToAttach) {
        childFolderToAttach.get()->setParentFolder(this);
        childrenFolder.push_back(std::move(childFolderToAttach));
    }

private:
    void setParentFolder(FolderTrack* newParentFolder) {
        parentFolder = newParentFolder;
    }

    std::vector<std::weak_ptr<Track>> tracks;
    FolderTrack* parentFolder;
    std::vector<std::shared_ptr<FolderTrack>> childrenFolder;
};
