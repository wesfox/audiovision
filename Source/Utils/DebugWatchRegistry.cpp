#include "DebugWatchRegistry.h"

DebugWatchRegistry& DebugWatchRegistry::get() {
    static DebugWatchRegistry instance;
    return instance;
}

void DebugWatchRegistry::setWatch(const juce::String& name, const void* ptr, ToString toString) {
    if (name.isEmpty()) {
        return;
    }
    const juce::ScopedLock scopedLock(lock);
    watches[name] = WatchEntry{ ptr, std::move(toString) };
}

void DebugWatchRegistry::clearWatch(const juce::String& name) {
    const juce::ScopedLock scopedLock(lock);
    watches.erase(name);
}

std::vector<DebugWatchRegistry::WatchLine> DebugWatchRegistry::snapshot() const {
    std::vector<std::pair<juce::String, WatchEntry>> entries;
    {
        const juce::ScopedLock scopedLock(lock);
        entries.reserve(watches.size());
        for (const auto& [name, entry] : watches) {
            entries.emplace_back(name, entry);
        }
    }

    std::vector<WatchLine> lines;
    lines.reserve(entries.size());
    for (const auto& [name, entry] : entries) {
        if (entry.ptr == nullptr || !entry.toString) {
            continue;
        }
        lines.push_back({ name, entry.toString(entry.ptr) });
    }
    return lines;
}
