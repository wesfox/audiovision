#include "VideoThumbnailCache.h"

VideoThumbnailCache::RequestJob::RequestJob(VideoThumbnailCache& cache,
                                            int64_t key,
                                            double seconds,
                                            uint32_t generation)
    : ThreadPoolJob("VideoThumbnailRequest"),
      cache(cache),
      key(key),
      seconds(seconds),
      generation(generation) {
}

juce::ThreadPoolJob::JobStatus VideoThumbnailCache::RequestJob::runJob() {
    if (shouldExit()) {
        return jobHasFinished;
    }
    juce::Image frame;
    if (cache.provider && cache.provider->isReady()) {
        frame = cache.provider->getFrameAtSeconds(seconds);
    }
    juce::Logger::writeToLog("ThumbnailCache: job finished for " + juce::String(seconds)
        + "s, frame valid: " + juce::String(static_cast<int>(frame.isValid())));
    cache.dispatchResult(key, generation, frame);
    return jobHasFinished;
}

void VideoThumbnailCache::setVideoFile(const VideoFile& file) {
    const auto nextFile = file.getFile();
    if (currentFile == nextFile) {
        return;
    }
    pool.removeAllJobs(true, 10000);
    {
        const juce::ScopedLock scopedLock(lock);
        cache.clear();
        pending.clear();
        currentFile = nextFile;
        ++generation;
    }
    if (provider) {
        provider->setVideoFile(file);
    }
}

void VideoThumbnailCache::requestFrameSeconds(double seconds,
                                              std::function<void(const juce::Image&)> callback) {
    if (!provider) {
        return;
    }
    const auto key = toKey(seconds);
    juce::Image cachedFrame;
    bool shouldQueue = false;
    {
        const juce::ScopedLock scopedLock(lock);
        const auto cachedIt = cache.find(key);
        if (cachedIt != cache.end()) {
            cachedFrame = cachedIt->second;
        } else {
            auto& entry = pending[key];
            if (entry.callbacks.empty()) {
                entry.seconds = seconds;
                shouldQueue = true;
            }
            entry.callbacks.push_back(std::move(callback));
        }
    }
    if (cachedFrame.isValid()) {
        juce::MessageManager::callAsync([callback = std::move(callback), cachedFrame]() mutable {
            callback(cachedFrame);
        });
        return;
    }
    if (shouldQueue) {
        pool.addJob(new RequestJob(*this, key, seconds, generation.load()), true);
    }
}

bool VideoThumbnailCache::isReady() const {
    return provider && provider->isReady();
}

void VideoThumbnailCache::dispatchResult(int64_t key, uint32_t requestGeneration,
                                         const juce::Image& frame) {
    std::vector<std::function<void(const juce::Image&)>> callbacks;
    {
        const juce::ScopedLock scopedLock(lock);
        if (requestGeneration != generation.load()) {
            juce::Logger::writeToLog("ThumbnailCache: drop stale result.");
            pending.erase(key);
            return;
        }
        const auto pendingIt = pending.find(key);
        if (pendingIt != pending.end()) {
            callbacks = std::move(pendingIt->second.callbacks);
            pending.erase(pendingIt);
        }
        if (frame.isValid()) {
            cache[key] = frame;
        }
    }
    if (callbacks.empty()) {
        juce::Logger::writeToLog("ThumbnailCache: no callbacks for result.");
        return;
    }
    juce::Logger::writeToLog("ThumbnailCache: dispatching callbacks, valid: "
        + juce::String(static_cast<int>(frame.isValid())));
    juce::MessageManager::callAsync([callbacks = std::move(callbacks), frame]() mutable {
        for (auto& callback : callbacks) {
            callback(frame);
        }
    });
}

int64_t VideoThumbnailCache::toKey(double seconds) {
    return static_cast<int64_t>(std::llround(seconds * 1000.0));
}
