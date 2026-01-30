#include "ThumbnailCache.h"

ThumbnailCache::ThumbnailCache()
    : cache(128) {
    formatManager.registerBasicFormats();
}

ThumbnailCache& ThumbnailCache::get() {
    static ThumbnailCache instance;
    return instance;
}

juce::AudioFormatManager& ThumbnailCache::getFormatManager() {
    return formatManager;
}

juce::AudioThumbnailCache& ThumbnailCache::getCache() {
    return cache;
}

int ThumbnailCache::getSamplesPerThumbSample() const {
    return samplesPerThumbSample;
}
