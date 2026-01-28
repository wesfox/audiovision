#pragma once

#include <JuceHeader.h>

#include "BinaryData.h"

/// Centralized font selectors for the UI.
namespace Fonts {
enum class Weight {
    Thin,
    Regular,
    Bold
};

inline juce::Typeface::Ptr loadRobotoTypeface(Weight weight) {
    const char* resourceName = nullptr;
    switch (weight) {
        case Weight::Thin:
            resourceName = "RobotoThin_ttf";
            break;
        case Weight::Bold:
            resourceName = "RobotoBold_ttf";
            break;
        case Weight::Regular:
        default:
            resourceName = "RobotoRegular_ttf";
            break;
    }
    int size = 0;
    auto* data = BinaryData::getNamedResource(resourceName, size);
    if (data == nullptr || size <= 0) {
        return {};
    }
    return juce::Typeface::createSystemTypefaceFor(data, size);
}

inline juce::Font h3(Weight weight = Weight::Bold) {
    static juce::Typeface::Ptr thinFace = loadRobotoTypeface(Weight::Thin);
    static juce::Typeface::Ptr regularFace = loadRobotoTypeface(Weight::Regular);
    static juce::Typeface::Ptr boldFace = loadRobotoTypeface(Weight::Bold);
    juce::Typeface::Ptr typeface;
    switch (weight) {
        case Weight::Thin:
            typeface = thinFace;
            break;
        case Weight::Bold:
            typeface = boldFace;
            break;
        case Weight::Regular:
        default:
            typeface = regularFace;
            break;
    }
    if (typeface != nullptr) {
        juce::Font font(typeface);
        font.setHeight(16.0f);
        return font;
    }
    return juce::Font(16.0f, weight == Weight::Bold ? juce::Font::bold : juce::Font::plain);
}

inline juce::Font p(float size, Weight weight = Weight::Regular) {
    static juce::Typeface::Ptr thinFace = loadRobotoTypeface(Weight::Thin);
    static juce::Typeface::Ptr regularFace = loadRobotoTypeface(Weight::Regular);
    static juce::Typeface::Ptr boldFace = loadRobotoTypeface(Weight::Bold);
    juce::Typeface::Ptr typeface;
    switch (weight) {
        case Weight::Thin:
            typeface = thinFace;
            break;
        case Weight::Bold:
            typeface = boldFace;
            break;
        case Weight::Regular:
        default:
            typeface = regularFace;
            break;
    }
    if (typeface != nullptr) {
        juce::Font font(typeface);
        font.setHeight(size);
        return font;
    }
    return juce::Font(size, weight == Weight::Bold ? juce::Font::bold : juce::Font::plain);
}

inline juce::Font p() {
    return p(14.0f, Weight::Regular);
}

inline juce::Font thin() {
    return p(14.0f, Weight::Thin);
}
}
