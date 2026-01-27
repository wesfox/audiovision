#include "SvgFactory.h"

#include "BinaryData.h"

std::optional<SvgFactory::SvgData> SvgFactory::getData(SVG_Assets asset) {
    int size = 0;
    switch (asset) {
        case SVG_Assets::TimelineCursorSvg: {
            auto* data = BinaryData::getNamedResource("TimelineCursor_svg", size);
            if (data == nullptr || size <= 0) {
                return std::nullopt;
            }
            return SvgData{ data, size };
        }
        default:
            return std::nullopt;
    }
}

std::unique_ptr<juce::Drawable> SvgFactory::load(SVG_Assets asset,
                                                 std::optional<juce::Colour> overrideFill) {
    const auto data = getData(asset);
    if (!data.has_value()) {
        return nullptr;
    }
    const auto xmlText = juce::String::fromUTF8(static_cast<const char*>(data->data), data->size);
    auto xml = juce::XmlDocument::parse(xmlText);
    if (xml == nullptr) {
        return nullptr;
    }

    auto drawable = juce::Drawable::createFromSVG(*xml);
    if (drawable == nullptr) {
        return nullptr;
    }

    if (overrideFill.has_value()) {
        drawable->replaceColour(juce::Colour::fromString("#4C2C7E"), *overrideFill);
    }

    return drawable;
}
