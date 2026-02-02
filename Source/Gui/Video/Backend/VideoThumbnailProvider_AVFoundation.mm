#import <AVFoundation/AVFoundation.h>

#include "VideoThumbnailProvider_AVFoundation.h"

#include <juce_core/juce_core.h>
#include <juce_graphics/juce_graphics.h>

namespace {
juce::Image imageFromCGImage(CGImageRef imageRef) {
    if (!imageRef) {
        return {};
    }
    const auto width = static_cast<int>(CGImageGetWidth(imageRef));
    const auto height = static_cast<int>(CGImageGetHeight(imageRef));
    if (width <= 0 || height <= 0) {
        return {};
    }
    juce::Image image(juce::Image::ARGB, width, height, true);
    juce::Image::BitmapData data(image, juce::Image::BitmapData::writeOnly);
    if (data.data == nullptr) {
        return {};
    }
    auto colourSpace = CGColorSpaceCreateDeviceRGB();
    auto bitmapInfo = static_cast<CGBitmapInfo>(kCGImageAlphaPremultipliedFirst | kCGBitmapByteOrder32Host);
    auto context = CGBitmapContextCreate(
        data.data,
        static_cast<size_t>(width),
        static_cast<size_t>(height),
        8,
        static_cast<size_t>(data.lineStride),
        colourSpace,
        bitmapInfo);
    CGColorSpaceRelease(colourSpace);
    if (!context) {
        return {};
    }
    CGContextDrawImage(context, CGRectMake(0.0, 0.0, width, height), imageRef);
    CGContextRelease(context);
    return image;
}
}

@interface AVFoundationThumbnailImpl : NSObject
@property (strong, nonatomic) AVAsset* asset;
@property (strong, nonatomic) AVAssetImageGenerator* generator;
@end

@implementation AVFoundationThumbnailImpl
@end

VideoThumbnailProvider_AVFoundation::VideoThumbnailProvider_AVFoundation() = default;

VideoThumbnailProvider_AVFoundation::~VideoThumbnailProvider_AVFoundation() {
    if (impl) {
        auto* state = (__bridge_transfer AVFoundationThumbnailImpl*)impl;
        state.asset = nil;
        state.generator = nil;
        impl = nullptr;
    }
}

void VideoThumbnailProvider_AVFoundation::setVideoFile(const VideoFile& file) {
    if (impl) {
        auto* state = (__bridge_transfer AVFoundationThumbnailImpl*)impl;
        state.asset = nil;
        state.generator = nil;
        impl = nullptr;
    }

    auto* state = [[AVFoundationThumbnailImpl alloc] init];
    const auto path = file.getFile().getFullPathName().toStdString();
    state.asset = [AVAsset assetWithURL:[NSURL fileURLWithPath:@(path.c_str())]];
    state.generator = [AVAssetImageGenerator assetImageGeneratorWithAsset:state.asset];
    state.generator.appliesPreferredTrackTransform = YES;
    state.generator.requestedTimeToleranceBefore = kCMTimeZero;
    state.generator.requestedTimeToleranceAfter = kCMTimeZero;
    impl = (__bridge_retained void*)state;
}

juce::Image VideoThumbnailProvider_AVFoundation::getFrameAtSeconds(double seconds) {
    if (!impl) {
        return {};
    }
    auto* state = (__bridge AVFoundationThumbnailImpl*)impl;
    if (!state.generator) {
        return {};
    }
    CMTime time = CMTimeMakeWithSeconds(seconds, 600);
    NSError* error = nil;
    CGImageRef imageRef = [state.generator copyCGImageAtTime:time actualTime:nullptr error:&error];
    if (!imageRef) {
        return {};
    }
    auto image = imageFromCGImage(imageRef);
    CGImageRelease(imageRef);
    return image;
}

bool VideoThumbnailProvider_AVFoundation::isReady() const {
    if (!impl) {
        return false;
    }
    auto* state = (__bridge AVFoundationThumbnailImpl*)impl;
    return state.generator != nil;
}
