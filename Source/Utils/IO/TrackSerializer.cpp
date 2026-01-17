#include "TrackSerializer.h"

juce::Array<TrackSerializer::ClipsWithMilliseconds> TrackSerializer::importFromFile(const juce::File& sessionFile)
{
    juce::Array<ClipsWithMilliseconds> tracks;

    if (!sessionFile.existsAsFile())
        return tracks;

    juce::String content = sessionFile.loadFileAsString();
    juce::StringArray lines;
    lines.addLines(content);

    for (auto line : lines)
    {
        line = line.trim();
        if (line.isEmpty() || line.startsWithChar('#'))
            continue;

        auto parts = juce::StringArray::fromTokens(line, "|", "");
        if (parts.size() == 0)
            continue;

        juce::File file(parts[0]);
        int startMs = 0;
        int endMs = 0;
        int timerefMs = 0;
        if (parts.size() > 1)
            startMs = parts[1].getIntValue();
        if (parts.size() > 2)
            endMs = parts[2].getIntValue();
        if (parts.size() > 3)
            timerefMs = parts[3].getIntValue();

        ClipsWithMilliseconds fwm;
        fwm.file = file;
        fwm.startMs = startMs;
        fwm.endMs = endMs;
        fwm.timerefMs = timerefMs;
        tracks.add(fwm);
    }

    juce::Logger::writeToLog("Imported session file: " + sessionFile.getFullPathName());
    return tracks;
}

bool TrackSerializer::exportToFile(const juce::Array<ClipsWithMilliseconds>& tracks, const juce::File& sessionFile)
{
    juce::String content;
    content << "# Session export - created: " << juce::Time::getCurrentTime().toString(true, true) << "\n";

    // Write selected files and their start/end/timeref milliseconds, one per line: path|start|end|timeref
    for (const auto& f : tracks)
    {
        content << f.file.getFullPathName() << "|" << juce::String(f.startMs) << "|" << juce::String(f.endMs) << "|" << juce::String(f.timerefMs) << "\n";
    }

    if (sessionFile.replaceWithText(content))
    {
        juce::Logger::writeToLog("Session file created: " + sessionFile.getFullPathName());
        return true;
    }
    else
    {
        juce::Logger::writeToLog("Failed to write session file: " + sessionFile.getFullPathName());
        return false;
    }
}