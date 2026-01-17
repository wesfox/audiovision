#pragma once

#include <JuceHeader.h>
#include "../../Utils/IO/TrackSerializer.h"

// ------------------------ File Selector Component ------------------------
class FileSelectorComponent : public juce::Component,
                              public juce::FileBrowserListener,
                              public juce::Button::Listener
{
public:
    FileSelectorComponent();
    ~FileSelectorComponent() override;

    void resized() override;
    void buttonClicked(juce::Button* button) override;

    // FileBrowserListener callbacks
    void selectionChanged() override;
    void fileClicked(const juce::File& file, const juce::MouseEvent& e) override;
    void fileDoubleClicked(const juce::File& file) override;
    void browserRootChanged(const juce::File& newRoot) override;

    // Callbacks
    std::function<void(const juce::File&)> onFileSelected = [](const juce::File&) {};  // Convenience callback (called when file is added)
    std::function<void(const juce::File&)> onFileAdded = [](const juce::File&) {};
    std::function<void(const juce::File&)> onFileRemoved = [](const juce::File&) {};
    std::function<void(const juce::Array<juce::File>&)> onSelectionChanged = [](const juce::Array<juce::File>&) {};

    juce::Array<juce::File> getSelectedFiles() const;
    void clearSelection();
    void setRootFolder(const juce::File& folder);
    
    // Get start/end milliseconds for a specific file
    int getStartMillisecondsForFile(const juce::File& file) const;
    int getEndMillisecondsForFile(const juce::File& file) const;
    // Backwards-compatible alias returning the start milliseconds
    int getMillisecondsForFile(const juce::File& file) const;

    // Simple payload object used when requesting a session play/import
    struct ImportedTrack
    {
        juce::Array<juce::File> files;
        juce::Array<int> starts;
        juce::Array<int> ends;
        juce::Array<int> timerefs;
    };

    // Callback fired when Play Session is requested. Provides an ImportedTrack payload.
    std::function<void(const ImportedTrack&)> onPlaySession = [](const ImportedTrack&) {};

private:
    using FileWithMilliseconds = TrackSerializer::ClipsWithMilliseconds;
    
    void addFileToSelection(const juce::File& file);
    void removeFileFromSelection(const juce::File& file);
    void updateSelectedFilesList();
    void updateStartForFile(const juce::File& file, int startMilliseconds);
    void updateEndForFile(const juce::File& file, int endMilliseconds);
    
    struct SelectedFileRow : public juce::Component,
                              public juce::TextEditor::Listener
    {
        SelectedFileRow(const juce::File& f, int startMs, int endMs, int timerefMs, FileSelectorComponent* parent);
        void resized() override;
        void textEditorTextChanged(juce::TextEditor& editor) override;
        
        juce::File file;
        juce::Label fileNameLabel;
        juce::TextEditor startInput;
        juce::Label startLabel;
        juce::TextEditor endInput;
        juce::Label endLabel;
        juce::TextEditor timerefInput;
        juce::Label timerefLabel;
        juce::TextButton removeButton;
        FileSelectorComponent* owner;
    };

    std::unique_ptr<juce::FileBrowserComponent> fileBrowser;
    juce::Label label;
    juce::Label selectedFilesLabel;
    juce::Viewport selectedFilesViewport;
    juce::Component selectedFilesContainer;
    
    juce::Array<FileWithMilliseconds> selectedFiles;
    juce::OwnedArray<SelectedFileRow> fileRows;
    juce::TextButton createSessionButton;
    juce::TextButton importSessionButton;
    juce::TextButton playSessionButton;

    void createSessionFile();
    void importSessionFile();
    void playSessionFile();
    void updateTimerefForFile(const juce::File& file, int timerefMilliseconds);
    int getTimerefForFile(const juce::File& file) const;
};
