#pragma once

#include <JuceHeader.h>

enum class PluginFormat {
    VST3
};

class Plugin{
public:
    Plugin(String name,
           String vendor,
           String path,
           PluginFormat format);

    String getName() const { return name; }
    String getVendor() const { return vendor; }
    String getPath() const { return path; }
    PluginFormat getFormat() const { return format; }

private:
    String name;
    String vendor;
    String path;
    PluginFormat format;
};
