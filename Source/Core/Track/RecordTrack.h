#pragma once

#include "AudioTrack.h"

/// Audio track intended for recording workflows.
class RecordTrack final : public AudioTrack {
public:
    /// Create a record track with an optional name.
    /// @param name display name
    explicit RecordTrack(const String& name = "");

    /// Create a shared record track.
    /// @param name display name
    static std::shared_ptr<RecordTrack> create(const String& name = "")
    {
        return std::make_shared<RecordTrack>(name);
    }
};
