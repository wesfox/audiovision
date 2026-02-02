#pragma once

#include <JuceHeader.h>

#include <functional>
#include <map>
#include <vector>

/// Stores debug watch entries for global inspection.
class DebugWatchRegistry {
public:
    /// Render callback for a watched resource.
    using ToString = std::function<juce::String(const void*)>;

    /// Snapshot line for UI rendering.
    struct WatchLine {
        juce::String name;
        juce::String value;
    };

    /// Access the global registry.
    static DebugWatchRegistry& get();

    /// Register or update a watch entry.
    /// @param name label to display
    /// @param ptr resource pointer (ignored when null)
    /// @param toString formatter for the resource
    void setWatch(const juce::String& name, const void* ptr, ToString toString);

    /// Remove a watch entry by name.
    /// @param name label to remove
    void clearWatch(const juce::String& name);

    /// Build a snapshot of the current watch values.
    std::vector<WatchLine> snapshot() const;

private:
    struct WatchEntry {
        const void* ptr = nullptr;
        ToString toString;
    };

    DebugWatchRegistry() = default;

    mutable juce::CriticalSection lock;
    std::map<juce::String, WatchEntry> watches;
};
