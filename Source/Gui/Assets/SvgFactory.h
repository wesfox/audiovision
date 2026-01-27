#pragma once

#include <JuceHeader.h>
#include <optional>

/// SVG assets available in the app.
enum class SvgAsset {
    TimelineCursorSvg
};

/// Factory for loading SVG assets from disk.
class SvgFactory {
public:
    /// Resolve the file path for an SVG asset.
    /// @param asset asset identifier
    static juce::File getFile(SvgAsset asset);

    /// Load an SVG asset as a drawable.
    /// @param asset asset identifier
    /// @param overrideFill optional fill colour override
    static std::unique_ptr<juce::Drawable> load(SvgAsset asset,
                                                std::optional<juce::Colour> overrideFill = std::nullopt);
};
