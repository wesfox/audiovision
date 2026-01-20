#include "FolderTrack.h"

FolderTrack::FolderTrack(FolderTrack* parentFolder, const String &name)
    : Track(name),parentFolder(parentFolder)
{
    trackType = TrackType::Folder;
}
