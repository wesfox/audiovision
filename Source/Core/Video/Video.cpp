#include "Video.h"

#include "VideoFile.h"

Video::Video() {
    auto file = std::make_shared<VideoFile>(juce::File("/Users/nico/Movies/Exemple_ProRes.mov"),
                                            "Anniv Victor");
    clips.emplace_back(std::move(file), 0, -1);
}

const std::vector<VideoClip>& Video::getClips() const {
    return clips;
}

void Video::addClip(VideoClip clip) {
    clips.push_back(std::move(clip));
}

const VideoClip* Video::findClipForFrame(int64 frame) const {
    for (const auto& clip : clips) {
        if (clip.containsFrame(frame)) {
            return &clip;
        }
    }
    return nullptr;
}
