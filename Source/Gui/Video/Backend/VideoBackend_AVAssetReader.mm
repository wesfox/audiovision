#import <AVFoundation/AVFoundation.h>
#import <CoreMedia/CoreMedia.h>
#import <CoreVideo/CoreVideo.h>

#include "VideoBackend_AVAssetReader.h"

#include <juce_core/juce_core.h>

#include <algorithm>
#include <cmath>
#include <cstring>

namespace {
juce::Image imageFromPixelBuffer(CVPixelBufferRef buffer) {
    if (buffer == nullptr) {
        return {};
    }
    CVPixelBufferLockBaseAddress(buffer, kCVPixelBufferLock_ReadOnly);
    const size_t width = CVPixelBufferGetWidth(buffer);
    const size_t height = CVPixelBufferGetHeight(buffer);
    const size_t bytesPerRow = CVPixelBufferGetBytesPerRow(buffer);
    auto* baseAddress = static_cast<uint8_t*>(CVPixelBufferGetBaseAddress(buffer));
    juce::Image image(juce::Image::ARGB, static_cast<int>(width), static_cast<int>(height), false);
    juce::Image::BitmapData data(image, juce::Image::BitmapData::writeOnly);
    for (size_t y = 0; y < height; ++y) {
        const auto* src = baseAddress + y * bytesPerRow;
        auto* dst = data.getLinePointer(static_cast<int>(y));
        std::memcpy(dst, src, std::min(static_cast<size_t>(data.lineStride), bytesPerRow));
    }
    CVPixelBufferUnlockBaseAddress(buffer, kCVPixelBufferLock_ReadOnly);
    return image;
}
}

@interface AVAssetReaderImpl : NSObject
@property (strong, nonatomic) AVAsset* asset;
@property (strong, nonatomic) AVAssetTrack* track;
@property (strong, nonatomic) AVAssetReader* reader;
@property (strong, nonatomic) AVAssetReaderTrackOutput* output;
@end

@implementation AVAssetReaderImpl
@end

VideoBackend_AVAssetReader::VideoBackend_AVAssetReader()
    : Thread("VideoReader") {
    impl = (__bridge_retained void*)[[AVAssetReaderImpl alloc] init];
}

VideoBackend_AVAssetReader::~VideoBackend_AVAssetReader() {
    stop();
    {
        const juce::ScopedLock scopedLock(lock);
        auto* state = (__bridge_transfer AVAssetReaderImpl*)impl;
        state.output = nil;
        state.reader = nil;
        state.track = nil;
        state.asset = nil;
        impl = nullptr;
    }
}

void VideoBackend_AVAssetReader::loadFile(const juce::File& file) {
    stop();
    const juce::ScopedLock scopedLock(lock);
    auto* state = (__bridge AVAssetReaderImpl*)impl;
    ready = false;
    frameBuffer.clear();
    lastFrame = {};
    nominalFrameRate = 0.0;
    NSURL* url = [NSURL fileURLWithPath:[NSString stringWithUTF8String:file.getFullPathName().toRawUTF8()]];
    state.asset = [AVAsset assetWithURL:url];
    NSArray<AVAssetTrack*>* tracks = [state.asset tracksWithMediaType:AVMediaTypeVideo];
    if (tracks.count == 0) {
        return;
    }
    state.track = tracks[0];
    nominalFrameRate = state.track.nominalFrameRate;
    ready = true;
    resetReader(targetSeconds);
}

void VideoBackend_AVAssetReader::setPlayheadSeconds(double seconds) {
    const juce::ScopedLock scopedLock(lock);
    targetSeconds = seconds;
    if (!playing) {
        resetReader(seconds);
        decodeNextFrame();
    }
}

void VideoBackend_AVAssetReader::setFrameRate(double newFrameRate) {
    frameRate = newFrameRate;
}

double VideoBackend_AVAssetReader::getPlayheadSeconds() const {
    const juce::ScopedLock scopedLock(lock);
    return targetSeconds;
}

void VideoBackend_AVAssetReader::play() {
    if (!ready) {
        return;
    }
    {
        const juce::ScopedLock scopedLock(lock);
        playing = true;
    }
    startThread();
}

void VideoBackend_AVAssetReader::stop() {
    {
        const juce::ScopedLock scopedLock(lock);
        playing = false;
    }
    stopThread(2000);
}

bool VideoBackend_AVAssetReader::isReady() const {
    const juce::ScopedLock scopedLock(lock);
    return ready;
}

bool VideoBackend_AVAssetReader::isPlaying() const {
    const juce::ScopedLock scopedLock(lock);
    return playing;
}

juce::Image VideoBackend_AVAssetReader::getCurrentFrameImage() {
    const juce::ScopedLock scopedLock(lock);
    if (frameRate <= 0.0) {
        return lastFrame;
    }
    const auto frameIndex = frameIndexForSeconds(targetSeconds);
    auto frame = frameBuffer.getExactFrame(frameIndex);
    if (frame.isValid()) {
        lastFrame = frame;
    }
    return lastFrame;
}

int64_t VideoBackend_AVAssetReader::getLastFrameIndex() const {
    const juce::ScopedLock scopedLock(lock);
    return frameBuffer.getLastFrameIndex();
}

double VideoBackend_AVAssetReader::getNominalFrameRate() const {
    const juce::ScopedLock scopedLock(lock);
    return nominalFrameRate;
}

void VideoBackend_AVAssetReader::run() {
    while (!threadShouldExit()) {
        int decodeBatch = 0;
        bool shouldWait = false;
        int waitMs = 0;
        {
            const juce::ScopedLock scopedLock(lock);
            if (!ready) {
                shouldWait = true;
                waitMs = 10;
            } else if (!playing && !prefillWhilePaused) {
                shouldWait = true;
                waitMs = 10;
            } else if (frameRate > 0.0) {
                const auto targetIndex = frameIndexForSeconds(targetSeconds);
                const auto firstIndex = frameBuffer.getFirstFrameIndex();
                const auto lastIndex = frameBuffer.getLastFrameIndex();
                const auto maxDelta = 1.0 / frameRate;
                const bool hasBuffer = frameBuffer.size() > 0;
                const bool targetBeforeBuffer = hasBuffer && targetIndex < firstIndex - 2;
                const bool targetAfterBuffer = hasBuffer && targetIndex > lastIndex + 2;
                const bool targetMovedWhileEmpty = !hasBuffer
                    && std::abs(targetSeconds - readerStartSeconds) > maxDelta * 2.0;
                if (targetBeforeBuffer || targetAfterBuffer || targetMovedWhileEmpty) {
                    resetReader(targetSeconds);
                }
                const auto desiredLead = static_cast<int64_t>(playing
                    ? kBufferCapacity / 2
                    : kBufferCapacity / 4);
                if (frameBuffer.size() >= kBufferCapacity && lastIndex >= targetIndex + desiredLead) {
                    shouldWait = true;
                    waitMs = 5;
                } else {
                    const auto burstSize = static_cast<int>(frameBuffer.size() < kBufferCapacity
                        ? kBufferCapacity / 2
                        : std::max<size_t>(1, kBufferCapacity / 5));
                    decodeBatch = std::max(1, burstSize);
                }
            } else {
                decodeBatch = 1;
            }
        }
        if (shouldWait) {
            wait(waitMs);
            continue;
        }
        for (int i = 0; i < decodeBatch; ++i) {
            if (!decodeNextFrame()) {
                wait(2);
                break;
            }
        }
    }
}

void VideoBackend_AVAssetReader::resetReader(double startSeconds) {
    auto* state = (__bridge AVAssetReaderImpl*)impl;
    if (!state.asset || !state.track) {
        return;
    }
    if (state.reader) {
        [state.reader cancelReading];
    }
    NSError* error = nil;
    state.reader = [[AVAssetReader alloc] initWithAsset:state.asset error:&error];
    NSDictionary* attrs = @{ (id)kCVPixelBufferPixelFormatTypeKey : @(kCVPixelFormatType_32BGRA) };
    state.output = [[AVAssetReaderTrackOutput alloc] initWithTrack:state.track outputSettings:attrs];
    state.output.alwaysCopiesSampleData = NO;
    state.reader.timeRange = CMTimeRangeMake(CMTimeMakeWithSeconds(startSeconds, 600), kCMTimePositiveInfinity);
    [state.reader addOutput:state.output];
    [state.reader startReading];
    readerStartSeconds = startSeconds;
    lastDecodedSeconds = startSeconds;
    frameBuffer.clear();
}

bool VideoBackend_AVAssetReader::decodeNextFrame() {
    auto* state = (__bridge AVAssetReaderImpl*)impl;
    if (!state.reader || !state.output) {
        return false;
    }
    CMSampleBufferRef sample = [state.output copyNextSampleBuffer];
    if (!sample) {
        return false;
    }
    const auto presentationTime = CMSampleBufferGetPresentationTimeStamp(sample);
    const auto seconds = CMTimeGetSeconds(presentationTime);
    CVPixelBufferRef buffer = CMSampleBufferGetImageBuffer(sample);
    auto image = imageFromPixelBuffer(buffer);
    const auto index = frameIndexForSeconds(seconds);
    {
        const juce::ScopedLock scopedLock(lock);
        frameBuffer.pushFrame(index, image);
        lastDecodedSeconds = seconds;
        if (!lastFrame.isValid()) {
            lastFrame = image;
        }
    }
    CFRelease(sample);
    return true;
}

int64_t VideoBackend_AVAssetReader::frameIndexForSeconds(double seconds) const {
    if (frameRate <= 0.0) {
        return 0;
    }
    return static_cast<int64_t>(std::llround(seconds * frameRate));
}
