#include "SvgFactory.h"

#include "BinaryData.h"

std::optional<SvgFactory::SvgData> SvgFactory::getData(SVG_Assets asset) {
    int size = 0;
    switch (asset) {
        case SVG_Assets::FaderButtonSvg: {
            auto* data = BinaryData::getNamedResource("faderbutton_svg", size);
            if (data == nullptr || size <= 0) {
                return std::nullopt;
            }
            return SvgData{ data, size };
        }
        case SVG_Assets::FaderSvg: {
            auto* data = BinaryData::getNamedResource("fader_svg", size);
            if (data == nullptr || size <= 0) {
                return std::nullopt;
            }
            return SvgData{ data, size };
        }
        case SVG_Assets::MoveLeftSvg: {
            auto* data = BinaryData::getNamedResource("moveleft_svg", size);
            if (data == nullptr || size <= 0) {
                return std::nullopt;
            }
            return SvgData{ data, size };
        }
        case SVG_Assets::MoveRightSvg: {
            auto* data = BinaryData::getNamedResource("moveright_svg", size);
            if (data == nullptr || size <= 0) {
                return std::nullopt;
            }
            return SvgData{ data, size };
        }
        case SVG_Assets::PlaySvg: {
            auto* data = BinaryData::getNamedResource("play_svg", size);
            if (data == nullptr || size <= 0) {
                return std::nullopt;
            }
            return SvgData{ data, size };
        }
        case SVG_Assets::RecordSvg: {
            auto* data = BinaryData::getNamedResource("record_svg", size);
            if (data == nullptr || size <= 0) {
                return std::nullopt;
            }
            return SvgData{ data, size };
        }
        case SVG_Assets::RewindSvg: {
            auto* data = BinaryData::getNamedResource("rewind_svg", size);
            if (data == nullptr || size <= 0) {
                return std::nullopt;
            }
            return SvgData{ data, size };
        }
        case SVG_Assets::StopSvg: {
            auto* data = BinaryData::getNamedResource("stop_svg", size);
            if (data == nullptr || size <= 0) {
                return std::nullopt;
            }
            return SvgData{ data, size };
        }
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
    auto xmlText = juce::String::fromUTF8(static_cast<const char*>(data->data), data->size);
    if (overrideFill.has_value()) {
        auto fillString = overrideFill->toDisplayString(false);
        if (!fillString.startsWithChar('#')) {
            fillString = "#" + fillString;
        }
        xmlText = xmlText.replace("currentColor", fillString);
    }
    auto xml = juce::XmlDocument::parse(xmlText);
    if (xml == nullptr) {
        return nullptr;
    }

    auto drawable = juce::Drawable::createFromSVG(*xml);
    if (drawable == nullptr) {
        return nullptr;
    }

    return drawable;
}
