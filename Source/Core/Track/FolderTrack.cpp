#include "FolderTrack.h"

FolderTrack::FolderTrack(FolderTrack* parentFolder, const String &name)
    : Track(name)
{
    setParentFolder(parentFolder);
    trackType = TrackType::Folder;
}
