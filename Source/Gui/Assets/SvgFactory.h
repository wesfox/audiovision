#pragma once

#include <JuceHeader.h>
#include <optional>

/// SVG assets available in the app.
enum class SVG_Assets {
    FaderButtonSvg,
    FaderSvg,
    RotaryFaderSvg,
    MoveLeftSvg,
    MoveRightSvg,
    PlaySvg,
    RecordSvg,
    RewindSvg,
    StopSvg,
    TimelineCursorSvg
};

/// Factory for loading SVG assets from disk.
class SvgFactory {
public:
    /// Byte data for an SVG asset.
    struct SvgData {
        const void* data = nullptr;
        int size = 0;
    };

    /// Resolve the embedded data for an SVG asset.
    /// @param asset asset identifier
    static std::optional<SvgData> getData(SVG_Assets asset);

    /// Load an SVG asset as a drawable.
    /// @param asset asset identifier
    /// @param overrideFill optional fill colour override
    static std::unique_ptr<juce::Drawable> load(SVG_Assets asset,
                                                std::optional<juce::Colour> overrideFill = std::nullopt);
};
