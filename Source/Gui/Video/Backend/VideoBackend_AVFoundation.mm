#import <AVFoundation/AVFoundation.h>
#import <CoreMedia/CoreMedia.h>
#import <CoreVideo/CoreVideo.h>
#import <QuartzCore/QuartzCore.h>

#include <juce_core/juce_core.h>
#include <juce_graphics/juce_graphics.h>

#include <cstring>
#include <limits>
#include <cmath>
#include <cmath>

#include "VideoBackend_AVFoundation.h"

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

@interface AVFoundationImpl : NSObject
@property (strong, nonatomic) AVPlayer* player;
@property (strong, nonatomic) AVPlayerItem* item;
@property (strong, nonatomic) AVPlayerItemVideoOutput* output;
@property (assign, nonatomic) BOOL ready;
@property (assign, nonatomic) BOOL playing;
@property (assign, nonatomic) double targetSeconds;
@end

@implementation AVFoundationImpl
@end

VideoBackend_AVFoundation::VideoBackend_AVFoundation() {
    impl = (__bridge_retained void*)[[AVFoundationImpl alloc] init];
}

VideoBackend_AVFoundation::~VideoBackend_AVFoundation() {
    auto* state = (__bridge_transfer AVFoundationImpl*)impl;
    if (state.item != nil) {
        [state.item removeOutput:state.output];
    }
    [state.player pause];
    state.player = nil;
    state.item = nil;
    state.output = nil;
}

void VideoBackend_AVFoundation::loadFile(const juce::File& file) {
    auto* state = (__bridge AVFoundationImpl*)impl;
    state.ready = false;
    @autoreleasepool {
        NSURL* url = [NSURL fileURLWithPath:[NSString stringWithUTF8String:file.getFullPathName().toRawUTF8()]];
        AVAsset* asset = [AVAsset assetWithURL:url];
        AVPlayerItem* item = [AVPlayerItem playerItemWithAsset:asset];
        NSDictionary* attrs = @{ (id)kCVPixelBufferPixelFormatTypeKey : @(kCVPixelFormatType_32BGRA) };
        AVPlayerItemVideoOutput* output = [[AVPlayerItemVideoOutput alloc] initWithPixelBufferAttributes:attrs];
        [item addOutput:output];
        state.item = item;
        state.output = output;
        state.player = [AVPlayer playerWithPlayerItem:item];
        state.targetSeconds = 0.0;
        state.playing = false;
        state.ready = true;
        nominalFrameRate = 0.0;
        NSArray<AVAssetTrack*>* tracks = [asset tracksWithMediaType:AVMediaTypeVideo];
        if (tracks.count > 0) {
            AVAssetTrack* track = tracks[0];
            nominalFrameRate = track.nominalFrameRate;
        }
    }
}

void VideoBackend_AVFoundation::setPlayheadSeconds(double seconds) {
    auto* state = (__bridge AVFoundationImpl*)impl;
    if (!state.player) {
        return;
    }
    state.targetSeconds = seconds;
    if (!state.playing) {
        updateFrameAtTime(seconds);
    }
}

void VideoBackend_AVFoundation::setFrameRate(double newFrameRate) {
    frameRate = newFrameRate;
}

double VideoBackend_AVFoundation::getPlayheadSeconds() const {
    auto* state = (__bridge AVFoundationImpl*)impl;
    if (!state.player) {
        return 0.0;
    }
    return state.targetSeconds;
}

void VideoBackend_AVFoundation::play() {
    auto* state = (__bridge AVFoundationImpl*)impl;
    if (state.player) {
        state.playing = true;
        [state.player play];
        updateFrameAtTime(state.targetSeconds);
    }
}

void VideoBackend_AVFoundation::stop() {
    auto* state = (__bridge AVFoundationImpl*)impl;
    if (state.player) {
        state.playing = false;
        [state.player pause];
    }
}

bool VideoBackend_AVFoundation::isReady() const {
    auto* state = (__bridge AVFoundationImpl*)impl;
    return state.ready && state.player != nil;
}

bool VideoBackend_AVFoundation::isPlaying() const {
    auto* state = (__bridge AVFoundationImpl*)impl;
    if (!state.player) {
        return false;
    }
    return state.playing;
}

juce::Image VideoBackend_AVFoundation::getCurrentFrameImage() {
    auto* state = (__bridge AVFoundationImpl*)impl;
    if (state.playing && state.targetSeconds != lastFrameSeconds) {
        updateFrameAtTime(state.targetSeconds);
    }
    return currentFrame;
}

void VideoBackend_AVFoundation::updateFrameAtTime(double seconds) {
    auto* state = (__bridge AVFoundationImpl*)impl;
    if (!state.output || !state.player) {
        return;
    }
    if (seconds == lastFrameSeconds) {
        return;
    }
    const auto time = CMTimeMakeWithSeconds(seconds, 600);
    bool shouldSeek = !state.playing;
    if (state.playing && frameRate > 0.0 && std::isfinite(lastFrameSeconds)) {
        const auto maxDelta = 2.0 / frameRate;
            if (std::abs(seconds - lastFrameSeconds) > maxDelta) {
                const auto now = CACurrentMediaTime();
                if (now - lastSeekHostTime > 0.2) {
                    shouldSeek = true;
                    lastSeekHostTime = now;
                    juce::Logger::writeToLog("VideoBackend: seek to " + juce::String(seconds));
                }
            }
        }
    if (shouldSeek) {
        [state.player seekToTime:time toleranceBefore:kCMTimeZero toleranceAfter:kCMTimeZero];
    }
    CMTime queryTime = time;
    if (state.playing && !shouldSeek) {
        const CFTimeInterval hostTime = CACurrentMediaTime();
        queryTime = [state.output itemTimeForHostTime:hostTime];
        if (!CMTIME_IS_VALID(queryTime)) {
            queryTime = time;
        }
    }
    const BOOL hasNew = [state.output hasNewPixelBufferForItemTime:queryTime];
    if (hasNew) {
        CMTime actualTime = kCMTimeInvalid;
        CVPixelBufferRef buffer = [state.output copyPixelBufferForItemTime:queryTime itemTimeForDisplay:&actualTime];
        double actualSeconds = seconds;
        if (CMTIME_IS_VALID(actualTime)) {
            actualSeconds = CMTimeGetSeconds(actualTime);
        }
        if (frameRate > 0.0) {
            const auto maxDelta = 1.0 / frameRate;
            const double compareSeconds = state.playing && CMTIME_IS_VALID(queryTime)
                ? CMTimeGetSeconds(queryTime)
                : seconds;
            if (std::abs(actualSeconds - compareSeconds) > maxDelta) {
                return;
            }
        }
        currentFrame = imageFromPixelBuffer(buffer);
        if (buffer) {
            CVBufferRelease(buffer);
        }
        if (frameRate > 0.0) {
            lastFrameIndex = static_cast<int64_t>(std::llround(actualSeconds * frameRate));
        }
        lastFrameSeconds = seconds;
    }
}

int64_t VideoBackend_AVFoundation::getLastFrameIndex() const {
    return lastFrameIndex;
}

double VideoBackend_AVFoundation::getNominalFrameRate() const {
    return nominalFrameRate;
}
