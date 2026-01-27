#include "SvgFactory.h"

namespace {
juce::File getProjectRoot() {
    return juce::File::getCurrentWorkingDirectory();
}
}

juce::File SvgFactory::getFile(SvgAsset asset) {
    const auto root = getProjectRoot();
    switch (asset) {
        case SvgAsset::TimelineCursorSvg:
            return root.getChildFile("Source/Gui/Assets/TimelineCursor.svg");
        default:
            return {};
    }
}

std::unique_ptr<juce::Drawable> SvgFactory::load(SvgAsset asset,
                                                 std::optional<juce::Colour> overrideFill) {
    auto file = getFile(asset);
    if (!file.existsAsFile()) {
        return nullptr;
    }

    auto xml = juce::XmlDocument::parse(file);
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
