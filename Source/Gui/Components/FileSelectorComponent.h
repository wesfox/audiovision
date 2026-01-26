#pragma once

#include <JuceHeader.h>
#include "../../Utils/IO/TrackSerializer.h"

/// File browser UI for building a session selection.
class FileSelectorComponent : public juce::Component,
                              public juce::FileBrowserListener,
                              public juce::Button::Listener
{
public:
    /// Create the file selector UI.
    FileSelectorComponent();
    ~FileSelectorComponent() override;

    /// Layout UI components.
    void resized() override;

    /// Handle button clicks from this component.
    /// @param button clicked button
    void buttonClicked(juce::Button* button) override;

    /// Handle file browser selection change.
    void selectionChanged() override;

    /// Handle file click in the browser.
    /// @param file clicked file
    /// @param e mouse event data
    void fileClicked(const juce::File& file, const juce::MouseEvent& e) override;

    /// Handle file double-click in the browser.
    /// @param file double-clicked file
    void fileDoubleClicked(const juce::File& file) override;

    /// Handle root folder changes.
    /// @param newRoot new root directory
    void browserRootChanged(const juce::File& newRoot) override;

    /// Callback when a file is selected.
    std::function<void(const juce::File&)> onFileSelected = [](const juce::File&) {};
    /// Callback when a file is added to the selection.
    std::function<void(const juce::File&)> onFileAdded = [](const juce::File&) {};
    /// Callback when a file is removed from the selection.
    std::function<void(const juce::File&)> onFileRemoved = [](const juce::File&) {};
    /// Callback when the selection set changes.
    std::function<void(const juce::Array<juce::File>&)> onSelectionChanged = [](const juce::Array<juce::File>&) {};

    /// Return the current file selection.
    juce::Array<juce::File> getSelectedFiles() const;

    /// Clear the current file selection.
    void clearSelection();

    /// Set the root folder for browsing.
    /// @param folder root directory
    void setRootFolder(const juce::File& folder);
    
    /// Get the start time (ms) for a file in the selection.
    /// @param file selected file
    int getStartMillisecondsForFile(const juce::File& file) const;

    /// Get the end time (ms) for a file in the selection.
    /// @param file selected file
    int getEndMillisecondsForFile(const juce::File& file) const;

    /// Backwards-compatible start time accessor.
    /// @param file selected file
    int getMillisecondsForFile(const juce::File& file) const;

    /// Payload describing selected clips and timing.
    struct ImportedTrack
    {
        juce::Array<juce::File> files;
        juce::Array<int> starts;
        juce::Array<int> ends;
        juce::Array<int> timerefs;
    };

    /// Callback fired when Play Session is requested.
    std::function<void(const ImportedTrack&)> onPlaySession = [](const ImportedTrack&) {};

private:
    using FileWithMilliseconds = TrackSerializer::ClipsWithMilliseconds;
    
    /// Add a file to the current selection.
    /// @param file file to add
    void addFileToSelection(const juce::File& file);

    /// Remove a file from the current selection.
    /// @param file file to remove
    void removeFileFromSelection(const juce::File& file);

    /// Refresh the selected files UI list.
    void updateSelectedFilesList();

    /// Update the start time for a file.
    /// @param file target file
    /// @param startMilliseconds new start time in ms
    void updateStartForFile(const juce::File& file, int startMilliseconds);

    /// Update the end time for a file.
    /// @param file target file
    /// @param endMilliseconds new end time in ms
    void updateEndForFile(const juce::File& file, int endMilliseconds);
    
    /// UI row for a selected file and its timing.
    struct SelectedFileRow : public juce::Component,
                              public juce::TextEditor::Listener
    {
        /// Create a row for a selected file.
        /// @param f file represented by this row
        /// @param startMs start time in ms
        /// @param endMs end time in ms
        /// @param timerefMs time reference in ms
        /// @param parent owning selector component
        SelectedFileRow(const juce::File& f, int startMs, int endMs, int timerefMs, FileSelectorComponent* parent);

        /// Layout row controls.
        void resized() override;

        /// Handle time edits.
        /// @param editor edited text field
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

    /// Create a session file from the current selection.
    void createSessionFile();

    /// Import a session file into the selection.
    void importSessionFile();

    /// Trigger playback of the current selection.
    void playSessionFile();

    /// Update time reference for a file.
    /// @param file target file
    /// @param timerefMilliseconds new time reference in ms
    void updateTimerefForFile(const juce::File& file, int timerefMilliseconds);

    /// Get time reference for a file.
    /// @param file target file
    int getTimerefForFile(const juce::File& file) const;
};
