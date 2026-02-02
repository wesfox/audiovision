#pragma once

#include <juce_core/juce_core.h>
#include <juce_graphics/juce_graphics.h>

#include "Core/Video/VideoFile.h"
#include "Gui/Video/Backend/VideoThumbnailProvider.h"
#include "Gui/Video/Backend/VideoThumbnailProviderFactory.h"

#include <functional>
#include <unordered_map>

/// Cache video thumbnails asynchronously for UI usage.
class VideoThumbnailCache {
public:
    ~VideoThumbnailCache();

    /// Request a thumbnail for the given file.
    /// @param file video file to read
    void setVideoFile(const VideoFile& file);

    /// Request a still frame at a given frame index.
    /// @param frameIndex frame index to fetch
    /// @param frameRate frames per second
    /// @param callback invoked on the message thread when ready
    void requestFrameIndex(int64_t frameIndex,
                           double frameRate,
                           std::function<void(const juce::Image&)> callback);

    /// Check whether the provider is ready.
    bool isReady() const;

    /// Cancel pending thumbnail work.
    void cancelPending();

private:
    struct PendingRequest {
        double seconds = 0.0;
        std::vector<std::function<void(const juce::Image&)>> callbacks;
    };

    class RequestJob : public juce::ThreadPoolJob {
    public:
        RequestJob(VideoThumbnailCache& cache,
                   int64_t key,
                   double seconds,
                   uint32_t generation);

        JobStatus runJob() override;

    private:
        VideoThumbnailCache& cache;
        int64_t key = 0;
        double seconds = 0.0;
        uint32_t generation = 0;
    };

    void dispatchResult(int64_t key, uint32_t generation, const juce::Image& frame);

    juce::ThreadPool pool { 1 };
    std::unique_ptr<VideoThumbnailProvider> provider = createVideoThumbnailProvider();
    juce::File currentFile;
    std::atomic<uint32_t> generation { 0 };
    mutable juce::CriticalSection lock;
    std::unordered_map<int64_t, juce::Image> cache;
    std::unordered_map<int64_t, PendingRequest> pending;
};
