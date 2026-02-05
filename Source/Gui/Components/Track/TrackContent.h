#pragma once

#include <JuceHeader.h>

#include "Core/Edit/Edit.h"
#include "Gui/Utils/SelectionManager.h"
#include "Gui/Utils/ViewRangeMapper.h"
#include "AudioClipComponent.h"
#include "Core/Track/AudioTrack.h"

class TrackContent : public juce::Component,
                     public juce::ValueTree::Listener,
                     private SelectionManager::Listener,
                     private AudioTrack::Listener {
public:
    TrackContent(Edit& edit, SelectionManager& selectionManager, std::shared_ptr<Track> track);
    ~TrackContent() override;

    void updateLayout();

    void resized() override;
    void paint(juce::Graphics& g) override;
    void paintOverChildren(juce::Graphics& g) override;
    void mouseDoubleClick(const juce::MouseEvent& event) override;
    void handleDoubleClick(int64 sample, bool extendSelection);
    void valueTreePropertyChanged(juce::ValueTree& tree, const juce::Identifier& property) override;

private:
    void clipsChanged(AudioTrack& track) override;
    void rebuildClipComponents();
    void applyWaveformScale();
    void selectionChanged() override;
    void setSelectedClipId(const String& clipId);
    void clearSelectedClip();
    void updateSelectedClipFromSelection();
    ViewRangeMapper getMapper() const;

    Edit& edit;
    SelectionManager& selectionManager;
    std::shared_ptr<Track> track;
    std::shared_ptr<AudioTrack> audioTrack;
    std::vector<std::unique_ptr<AudioClipComponent>> clipComponents;
    bool isSelected = false;
    String selectedClipId;
    bool suppressNextCursorClear = false;
};
