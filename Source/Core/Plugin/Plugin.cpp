#include "Plugin.h"

// ------------------------ MainComponent Implementation ------------------------

Plugin::Plugin(String name,
               String vendor,
               String path,
               PluginFormat format)
    : name(std::move(name)),
      vendor(std::move(vendor)),
      path(std::move(path)),
      format(format)
{
}
