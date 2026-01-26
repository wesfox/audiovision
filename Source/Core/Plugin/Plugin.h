#pragma once

#include <JuceHeader.h>

/// Supported plugin formats.
enum class PluginFormat {
    VST3
};

/// Plugin metadata used for instantiation and UI.
class Plugin{
public:
    /// Create plugin metadata.
    /// @param name plugin display name
    /// @param vendor plugin vendor
    /// @param path plugin file path
    /// @param format plugin format
    Plugin(String name,
           String vendor,
           String path,
           PluginFormat format);

    /// Plugin display name.
    String getName() const { return name; }

    /// Plugin vendor.
    String getVendor() const { return vendor; }

    /// Plugin file path.
    String getPath() const { return path; }

    /// Plugin format.
    PluginFormat getFormat() const { return format; }

private:
    String name;
    String vendor;
    String path;
    PluginFormat format;
};
