#include "EditSerializer.h"

#include "Core/Edit/Edit.h"
#include "Core/Edit/Scene.h"
#include "Core/Track/AudioTrack.h"
#include "Core/Track/AuxTrack.h"
#include "Core/Track/FolderTrack.h"
#include "Core/Track/Track.h"
#include "Core/Track/Send.h"
#include "Core/Plugin/Plugin.h"
#include "Core/AudioClip/AudioClip.h"

#include <Utils/IO/AudioFile.h>

namespace {
juce::String trackTypeToString(TrackType type)
{
    switch (type) {
        case TrackType::Audio: return "Audio";
        case TrackType::Aux: return "Aux";
        case TrackType::Folder: return "Folder";
        case TrackType::Unknown: return "Unknown";
    }
    return "Unknown";
}

TrackType trackTypeFromString(const juce::String& type)
{
    if (type == "Audio") return TrackType::Audio;
    if (type == "Aux") return TrackType::Aux;
    if (type == "Folder") return TrackType::Folder;
    return TrackType::Unknown;
}

juce::String pluginFormatToString(PluginFormat format)
{
    switch (format) {
        case PluginFormat::VST3: return "VST3";
    }
    return "VST3";
}

PluginFormat pluginFormatFromString(const juce::String& format)
{
    if (format == "VST3") return PluginFormat::VST3;
    return PluginFormat::VST3;
}
}

bool EditSerializer::exportToFile(const Edit& edit, const juce::File& directory)
{
    auto root = std::make_unique<juce::DynamicObject>();

    juce::Array<juce::var> tracksArray;
    const auto& tracks = const_cast<Edit&>(edit).getTracks();
    for (int i = 0; i < static_cast<int>(tracks.size()); ++i) {
        const auto& track = tracks[i];
        auto trackObj = std::make_unique<juce::DynamicObject>();
        trackObj->setProperty("index", i);
        trackObj->setProperty("name", track->getName());
        trackObj->setProperty("type", trackTypeToString(track->getTrackType()));
        trackObj->setProperty("armed", track->isAudioTrack()
            ? std::dynamic_pointer_cast<AudioTrack>(track)->isArmed()
            : false);

        auto output = track->getOutput().lock();
        if (output) {
            const auto& allTracks = tracks;
            int outputIndex = -1;
            for (int j = 0; j < static_cast<int>(allTracks.size()); ++j) {
                if (allTracks[j] == output) {
                    outputIndex = j;
                    break;
                }
            }
            trackObj->setProperty("outputIndex", outputIndex);
        } else {
            trackObj->setProperty("outputIndex", -1);
        }

        juce::Array<juce::var> clipsArray;
        if (track->getTrackType() == TrackType::Audio) {
            auto audioTrack = std::dynamic_pointer_cast<AudioTrack>(track);
            if (audioTrack) {
                for (const auto& clip : audioTrack->getAudioClips()) {
                    auto clipObj = std::make_unique<juce::DynamicObject>();
                    clipObj->setProperty("filePath", clip->getAudioFilePath());
                    clipObj->setProperty("fileStartSample", (int64)clip->getFileStartSample());
                    clipObj->setProperty("sessionStartSample", (int64)clip->getSessionStartSample());
                    clipObj->setProperty("sessionEndSample", (int64)clip->getSessionEndSample());
                    clipsArray.add(juce::var(clipObj.release()));
                }
            }
        }
        trackObj->setProperty("clips", clipsArray);

        juce::Array<juce::var> pluginsArray;
        for (const auto& plugin : track->getPlugins()) {
            if (!plugin) {
                continue;
            }
            auto pluginObj = std::make_unique<juce::DynamicObject>();
            pluginObj->setProperty("name", plugin->getName());
            pluginObj->setProperty("vendor", plugin->getVendor());
            pluginObj->setProperty("path", plugin->getPath());
            pluginObj->setProperty("format", pluginFormatToString(plugin->getFormat()));
            pluginsArray.add(juce::var(pluginObj.release()));
        }
        trackObj->setProperty("plugins", pluginsArray);

        juce::Array<juce::var> sendsArray;
        for (const auto& send : track->getSends()) {
            auto dest = send->getDestinationTrack().lock();
            if (!dest) {
                continue;
            }
            int destIndex = -1;
            for (int j = 0; j < static_cast<int>(tracks.size()); ++j) {
                if (tracks[j] == dest) {
                    destIndex = j;
                    break;
                }
            }
            auto sendObj = std::make_unique<juce::DynamicObject>();
            sendObj->setProperty("destinationIndex", destIndex);
            sendsArray.add(juce::var(sendObj.release()));
        }
        trackObj->setProperty("sends", sendsArray);

        tracksArray.add(juce::var(trackObj.release()));
    }
    root->setProperty("tracks", tracksArray);

    juce::Array<juce::var> scenesArray;
    const auto& scenes = const_cast<Edit&>(edit).getScenes();
    for (const auto& scene : scenes) {
        auto sceneObj = std::make_unique<juce::DynamicObject>();
        sceneObj->setProperty("name", scene->getName());
        sceneObj->setProperty("startSample", (int64)scene->sceneStartSample);
        sceneObj->setProperty("endSample", (int64)scene->sceneEndSample);
        juce::Array<juce::var> trackIds;
        for (const auto& trackRef : scene->getTracks()) {
            if (auto track = trackRef.lock()) {
                int trackIndex = -1;
                for (int i = 0; i < static_cast<int>(tracks.size()); ++i) {
                    if (tracks[i] == track) {
                        trackIndex = i;
                        break;
                    }
                }
                trackIds.add(trackIndex);
            }
        }
        sceneObj->setProperty("trackIndices", trackIds);
        scenesArray.add(juce::var(sceneObj.release()));
    }
    root->setProperty("scenes", scenesArray);

    int outputIndex = -1;
    if (auto outputTrack = edit.getAudioOutputTrack().lock()) {
        for (int i = 0; i < static_cast<int>(tracks.size()); ++i) {
            if (tracks[i] == outputTrack) {
                outputIndex = i;
                break;
            }
        }
    }
    root->setProperty("audioOutputIndex", outputIndex);

    const auto format = juce::JSON::FormatOptions()
        .withSpacing(juce::JSON::Spacing::multiLine);
    const auto json = juce::JSON::toString(juce::var(root.release()), format);
    if (juce::File(directory).getChildFile("audiovision.json").create().failed()) {
        juce::Logger::writeToLog("Error while creating export file");
        jassert(false);
    }
    auto exportFile = juce::File(directory).getChildFile("audiovision.json");
    return exportFile.replaceWithText(json);
}

std::shared_ptr<Edit> EditSerializer::importFromFile(const juce::File& file)
{
    if (!file.existsAsFile()) {
        return {};
    }
    auto text = file.loadFileAsString();
    auto rootVar = juce::JSON::parse(text);
    if (!rootVar.isObject()) {
        return {};
    }

    auto edit = std::make_shared<Edit>();
    auto* rootObj = rootVar.getDynamicObject();
    auto tracksVar = rootObj->getProperty("tracks");
    std::vector<std::shared_ptr<Track>> createdTracks;

    if (tracksVar.isArray()) {
        auto* tracksArray = tracksVar.getArray();
        createdTracks.reserve(static_cast<size_t>(tracksArray->size()));
        for (const auto& entry : *tracksArray) {
            if (!entry.isObject()) {
                continue;
            }
            auto* trackObj = entry.getDynamicObject();
            auto type = trackTypeFromString(trackObj->getProperty("type").toString());
            auto name = trackObj->getProperty("name").toString();

            std::shared_ptr<Track> track;
            switch (type) {
                case TrackType::Audio:
                    track = AudioTrack::create(name);
                    break;
                case TrackType::Aux:
                    track = AuxTrack::create(name);
                    break;
                case TrackType::Folder:
                    track = FolderTrack::create(nullptr, name);
                    break;
                default:
                    track = std::make_shared<Track>(name);
                    break;
            }

            if (auto audioTrack = std::dynamic_pointer_cast<AudioTrack>(track)) {
                audioTrack->setArmed(static_cast<bool>(trackObj->getProperty("armed")));
            }

            createdTracks.push_back(track);
            edit->addTrack(track);
        }

        for (int i = 0; i < tracksArray->size(); ++i) {
            auto* trackObj = tracksArray->getUnchecked(i).getDynamicObject();
            auto track = createdTracks[static_cast<size_t>(i)];

            const int outputIndex = (int)trackObj->getProperty("outputIndex");
            if (outputIndex >= 0 && outputIndex < (int)createdTracks.size()) {
                track->setOutput(createdTracks[static_cast<size_t>(outputIndex)]);
            }

            if (track->getTrackType() == TrackType::Audio) {
                auto audioTrack = std::dynamic_pointer_cast<AudioTrack>(track);
                if (audioTrack) {
                    auto clipsVar = trackObj->getProperty("clips");
                    if (clipsVar.isArray()) {
                        for (const auto& clipVar : *clipsVar.getArray()) {
                            if (!clipVar.isObject()) {
                                continue;
                            }
                            auto* clipObj = clipVar.getDynamicObject();
                            auto filePath = clipObj->getProperty("filePath").toString();
                            auto audioFile = AudioFile::get(juce::File(filePath));
                            auto clip = AudioClip::create(
                                audioFile,
                                (int64)clipObj->getProperty("fileStartSample"),
                                (int64)clipObj->getProperty("sessionStartSample"),
                                (int64)clipObj->getProperty("sessionEndSample"));
                            audioTrack->addAudioClip(std::move(clip));
                        }
                    }
                }
            }

            auto pluginsVar = trackObj->getProperty("plugins");
            if (pluginsVar.isArray()) {
                for (const auto& pluginVar : *pluginsVar.getArray()) {
                    if (!pluginVar.isObject()) {
                        continue;
                    }
                    auto* pluginObj = pluginVar.getDynamicObject();
                    auto plugin = std::make_shared<Plugin>(
                        pluginObj->getProperty("name").toString(),
                        pluginObj->getProperty("vendor").toString(),
                        pluginObj->getProperty("path").toString(),
                        pluginFormatFromString(pluginObj->getProperty("format").toString()));
                    track->addPlugin(std::move(plugin));
                }
            }

            auto sendsVar = trackObj->getProperty("sends");
            if (sendsVar.isArray()) {
                for (const auto& sendVar : *sendsVar.getArray()) {
                    if (!sendVar.isObject()) {
                        continue;
                    }
                    auto* sendObj = sendVar.getDynamicObject();
                    int destIndex = (int)sendObj->getProperty("destinationIndex");
                    if (destIndex >= 0 && destIndex < (int)createdTracks.size()) {
                        auto dest = createdTracks[static_cast<size_t>(destIndex)];
                        track->addSend(Send::create(dest));
                    }
                }
            }
        }
    }

    auto scenesVar = rootObj->getProperty("scenes");
    if (scenesVar.isArray()) {
        for (const auto& sceneVar : *scenesVar.getArray()) {
            if (!sceneVar.isObject()) {
                continue;
            }
            auto* sceneObj = sceneVar.getDynamicObject();
            auto scene = std::make_shared<Scene>(
                (int64)sceneObj->getProperty("startSample"),
                (int64)sceneObj->getProperty("endSample"),
                sceneObj->getProperty("name").toString());
            auto trackIndicesVar = sceneObj->getProperty("trackIndices");
            if (trackIndicesVar.isArray()) {
                for (const auto& indexVar : *trackIndicesVar.getArray()) {
                    int index = (int)indexVar;
                    if (index >= 0 && index < (int)createdTracks.size()) {
                        scene->addTrack(createdTracks[static_cast<size_t>(index)]);
                    }
                }
            }
            edit->addScene(scene);
        }
    }

    const int audioOutputIndex = (int)rootObj->getProperty("audioOutputIndex");
    if (audioOutputIndex >= 0 && audioOutputIndex < (int)createdTracks.size()) {
        edit->setAudioOutputTrack(createdTracks[static_cast<size_t>(audioOutputIndex)]);
    }

    return edit;
}
