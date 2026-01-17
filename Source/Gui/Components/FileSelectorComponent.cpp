#include "FileSelectorComponent.h"
#include <Utils/IO/TrackSerializer.h>

// ------------------------ File Selector Component Implementation ------------------------

// SelectedFileRow implementation
FileSelectorComponent::SelectedFileRow::SelectedFileRow(const juce::File& f, int startMs, int endMs, int timerefMs, FileSelectorComponent* parent)
    : file(f), owner(parent)
{
    fileNameLabel.setText(file.getFileName(), juce::dontSendNotification);
    addAndMakeVisible(&fileNameLabel);

    startLabel.setText("start:", juce::dontSendNotification);
    addAndMakeVisible(&startLabel);

    startInput.setText(juce::String(startMs), juce::dontSendNotification);
    startInput.setInputFilter(new juce::TextEditor::LengthAndCharacterRestriction(10, "0123456789"), true);
    startInput.addListener(this);
    addAndMakeVisible(&startInput);

    endLabel.setText("end:", juce::dontSendNotification);
    addAndMakeVisible(&endLabel);

    endInput.setText(juce::String(endMs), juce::dontSendNotification);
    endInput.setInputFilter(new juce::TextEditor::LengthAndCharacterRestriction(10, "0123456789"), true);
    endInput.addListener(this);
    addAndMakeVisible(&endInput);

    timerefLabel.setText("timeref:", juce::dontSendNotification);
    addAndMakeVisible(&timerefLabel);

    timerefInput.setText(juce::String(timerefMs), juce::dontSendNotification);
    timerefInput.setInputFilter(new juce::TextEditor::LengthAndCharacterRestriction(10, "0123456789"), true);
    timerefInput.addListener(this);
    addAndMakeVisible(&timerefInput);

    // timeref input
    endInput.setTooltip("End (ms)");
    // we'll set timeref up in updateSelectedFilesList when constructing rows

    removeButton.setButtonText("Remove");
    removeButton.onClick = [this]() {
        if (owner != nullptr)
            owner->removeFileFromSelection(file);
    };
    addAndMakeVisible(&removeButton);
}

void FileSelectorComponent::SelectedFileRow::resized()
{
    auto bounds = getLocalBounds();
    removeButton.setBounds(bounds.removeFromRight(80).reduced(2));
    
    auto timesArea = bounds.removeFromRight(360);
    startLabel.setBounds(timesArea.removeFromLeft(40).reduced(2));
    startInput.setBounds(timesArea.removeFromLeft(100).reduced(2));
    endLabel.setBounds(timesArea.removeFromLeft(40).reduced(2));
    endInput.setBounds(timesArea.removeFromLeft(100).reduced(2));
    timerefLabel.setBounds(timesArea.removeFromLeft(50).reduced(2));
    timerefInput.setBounds(timesArea.reduced(2));

    fileNameLabel.setBounds(bounds.reduced(5, 0));
}

void FileSelectorComponent::SelectedFileRow::textEditorTextChanged(juce::TextEditor& editor)
{
    if (&editor == &startInput && owner != nullptr)
    {
        int ms = startInput.getText().getIntValue();
        owner->updateStartForFile(file, ms);
    }
    else if (&editor == &endInput && owner != nullptr)
    {
        int ms = endInput.getText().getIntValue();
        owner->updateEndForFile(file, ms);
    }
    else if (&editor == &timerefInput && owner != nullptr)
    {
        int ms = timerefInput.getText().getIntValue();
        owner->updateTimerefForFile(file, ms);
    }
}

// FileSelectorComponent implementation
FileSelectorComponent::FileSelectorComponent()
{
    // Set up file browser
    fileBrowser = std::make_unique<juce::FileBrowserComponent>(
        juce::FileBrowserComponent::FileChooserFlags::openMode |
        juce::FileBrowserComponent::FileChooserFlags::canSelectFiles,
        juce::File::getSpecialLocation(juce::File::userHomeDirectory),
        nullptr,
        nullptr
    );
    
    fileBrowser->addListener(this);
    addAndMakeVisible(fileBrowser.get());
    
    // Add label
    label.setText("Select files:", juce::dontSendNotification);
    label.attachToComponent(fileBrowser.get(), false);
    addAndMakeVisible(&label);
    
    // Add selected files label
    selectedFilesLabel.setText("Selected files:", juce::dontSendNotification);
    addAndMakeVisible(&selectedFilesLabel);
    
    // Set up viewport for selected files
    selectedFilesViewport.setViewedComponent(&selectedFilesContainer, false);
    selectedFilesViewport.setScrollBarsShown(true, false);
    addAndMakeVisible(&selectedFilesViewport);

    // Create session button
    createSessionButton.setButtonText("Create session");
    createSessionButton.onClick = [this]() { createSessionFile(); };
    addAndMakeVisible(&createSessionButton);
    
    // Import session button
    importSessionButton.setButtonText("Import session");
    importSessionButton.onClick = [this]() { importSessionFile(); };
    addAndMakeVisible(&importSessionButton);
    
    // Play session button
    playSessionButton.setButtonText("Play Session");
    playSessionButton.onClick = [this]() { playSessionFile(); };
    addAndMakeVisible(&playSessionButton);
}

FileSelectorComponent::~FileSelectorComponent()
{
    fileBrowser->removeListener(this);
}

void FileSelectorComponent::resized()
{
    auto bounds = getLocalBounds();
    auto labelHeight = 30;
    auto selectedFilesHeight = 150;
    
    label.setBounds(bounds.removeFromTop(labelHeight));
    bounds.removeFromTop(15);
    
    auto selectedFilesArea = bounds.removeFromBottom(selectedFilesHeight);
    selectedFilesLabel.setBounds(selectedFilesArea.removeFromTop(labelHeight));
    // place the import and create session buttons to the right of the label
    {
        auto header = selectedFilesArea.removeFromTop(labelHeight);
        auto buttonsArea = header.removeFromRight(450);
        auto createArea = buttonsArea.removeFromRight(150);
        auto importArea = buttonsArea.removeFromRight(150);
        auto playArea = buttonsArea.removeFromRight(150);
        selectedFilesLabel.setBounds(header.reduced(2));
        createSessionButton.setBounds(createArea.reduced(8));
        importSessionButton.setBounds(importArea.reduced(8));
        playSessionButton.setBounds(playArea.reduced(8));
    }

    selectedFilesArea.removeFromTop(5);

    selectedFilesViewport.setBounds(selectedFilesArea);
    selectedFilesContainer.setSize(selectedFilesViewport.getWidth() - selectedFilesViewport.getScrollBarThickness(), selectedFilesContainer.getHeight());
    
    fileBrowser->setBounds(bounds);
    
    updateSelectedFilesList();
}

void FileSelectorComponent::importSessionFile()
{
    auto f = fileBrowser->getSelectedFile(0);
    if (!f.existsAsFile())
        return;

    selectedFiles = TrackSerializer::importFromFile(f);

    updateSelectedFilesList();

    juce::Array<juce::File> files;
    for (const auto& fw : selectedFiles)
        files.add(fw.file);
    onSelectionChanged(files);

    juce::Logger::writeToLog("Imported session file: " + f.getFullPathName());
}

void FileSelectorComponent::createSessionFile()
{
    // Create a "session" directory at the current working directory (project root when launched there)

    juce::File sessionDir = juce::File("~/code/TestLundi").getChildFile("session");
    if (!sessionDir.exists())
    {
        if (!sessionDir.createDirectory())
        {
            juce::Logger::writeToLog("Failed to create session directory: " + sessionDir.getFullPathName());
            return;
        }
    }

    // Build filename with timestamp
    auto timestamp = juce::Time::getCurrentTime().toISO8601(true);
    // replace ':' which can be problematic in filenames on some platforms
    timestamp = timestamp.replaceCharacter(':', '-');
    juce::String filename = "session_" + timestamp + ".txt";
    juce::File outFile = sessionDir.getChildFile(filename);

    TrackSerializer::exportToFile(selectedFiles, outFile);
}

void FileSelectorComponent::playSessionFile()
{
    ImportedTrack track;
    for (const auto& f : selectedFiles)
    {
        track.files.add(f.file);
        track.starts.add(f.startMs);
        track.ends.add(f.endMs);
        track.timerefs.add(f.timerefMs);
    }

    juce::Logger::writeToLog("Play Session requested: " + juce::String(track.files.size()) + " file(s)");
    for (int i = 0; i < track.files.size(); ++i)
    {
        juce::Logger::writeToLog("  " + track.files[i].getFullPathName() + " | start=" + juce::String(track.starts[i]) + " end=" + juce::String(track.ends[i]) + " timeref=" + juce::String(track.timerefs[i]));
    }

    onPlaySession(track);
}

void FileSelectorComponent::buttonClicked(juce::Button* button)
{
    // Handled by SelectedFileRow's onClick lambda
}

void FileSelectorComponent::selectionChanged()
{
    auto selectedFile = fileBrowser->getSelectedFile(0);
    juce::Logger::writeToLog("Selection changed callback - File: " + selectedFile.getFullPathName() + 
                             " exists: " + juce::String(selectedFile.exists() ? "yes" : "no") +
                             " isFile: " + juce::String(selectedFile.existsAsFile() ? "yes" : "no"));
}

void FileSelectorComponent::fileClicked(const juce::File& file, const juce::MouseEvent& e)
{
    juce::Logger::writeToLog("File clicked: " + file.getFullPathName() + 
                             " (exists: " + juce::String(file.exists() ? "yes" : "no") +
                             ", isFile: " + juce::String(file.existsAsFile() ? "yes" : "no") + ")");

    if (file.isDirectory())
    {
        juce::Logger::writeToLog("Clicked item is a directory, not adding to selection");
    }
}

void FileSelectorComponent::fileDoubleClicked(const juce::File& file)
{
    if (file.existsAsFile())
    {
        addFileToSelection(file);
    }
}

void FileSelectorComponent::browserRootChanged(const juce::File&)
{
}

void FileSelectorComponent::addFileToSelection(const juce::File& file)
{
    // Validate file exists and is actually a file
    if (!file.existsAsFile())
    {
        juce::Logger::writeToLog("Cannot add - not a valid file: " + file.getFullPathName());
        return;
    }
    
    // Check if file is already selected (compare by full path)
    for (const auto& fileWithMs : selectedFiles)
    {
        if (fileWithMs.file.getFullPathName() == file.getFullPathName())
        {
            juce::Logger::writeToLog("File already selected: " + file.getFullPathName());
            return;
        }
    }
    
    juce::Logger::writeToLog("Adding file to selection: " + file.getFullPathName());
    FileWithMilliseconds fileWithMs;
    fileWithMs.file = file;
    fileWithMs.startMs = 0; // Default start
    fileWithMs.endMs = 0;   // Default end
    fileWithMs.timerefMs = 0; // Default timeref
    selectedFiles.add(fileWithMs);
    updateSelectedFilesList();
    onFileSelected(file);  // Convenience callback for backward compatibility
    onFileAdded(file);
    
    // Extract files for callback
    juce::Array<juce::File> files;
    for (const auto& f : selectedFiles)
        files.add(f.file);
    onSelectionChanged(files);
}

void FileSelectorComponent::removeFileFromSelection(const juce::File& file)
{
    for (int i = selectedFiles.size() - 1; i >= 0; --i)
    {
        if (selectedFiles[i].file.getFullPathName() == file.getFullPathName())
        {
            selectedFiles.remove(i);
            break;
        }
    }
    updateSelectedFilesList();
    onFileRemoved(file);
    
    // Extract files for callback
    juce::Array<juce::File> files;
    for (const auto& f : selectedFiles)
        files.add(f.file);
    onSelectionChanged(files);
}

void FileSelectorComponent::updateSelectedFilesList()
{
    fileRows.clear();
    selectedFilesContainer.removeAllChildren();
    
    int rowHeight = 30;
    int y = 0;
    int containerWidth = selectedFilesViewport.getWidth() - selectedFilesViewport.getScrollBarThickness();
    if (containerWidth <= 0)
        containerWidth = 200; // Default width if viewport not sized yet
    
    for (const auto& fileWithMs : selectedFiles)
    {
        auto* row = new SelectedFileRow(fileWithMs.file, fileWithMs.startMs, fileWithMs.endMs, fileWithMs.timerefMs, this);
        row->setBounds(0, y, containerWidth, rowHeight);
        selectedFilesContainer.addAndMakeVisible(row);
        fileRows.add(row);
        y += rowHeight;
    }
    
    selectedFilesContainer.setSize(containerWidth, y);
}

void FileSelectorComponent::updateStartForFile(const juce::File& file, int startMilliseconds)
{
    for (auto& fileWithMs : selectedFiles)
    {
        if (fileWithMs.file.getFullPathName() == file.getFullPathName())
        {
            fileWithMs.startMs = startMilliseconds;
            juce::Logger::writeToLog("Updated start for " + file.getFileName() + ": " + juce::String(startMilliseconds));
            return;
        }
    }
}

void FileSelectorComponent::updateEndForFile(const juce::File& file, int endMilliseconds)
{
    for (auto& fileWithMs : selectedFiles)
    {
        if (fileWithMs.file.getFullPathName() == file.getFullPathName())
        {
            fileWithMs.endMs = endMilliseconds;
            juce::Logger::writeToLog("Updated end for " + file.getFileName() + ": " + juce::String(endMilliseconds));
            return;
        }
    }
}

void FileSelectorComponent::updateTimerefForFile(const juce::File& file, int timerefMilliseconds)
{
    for (auto& fileWithMs : selectedFiles)
    {
        if (fileWithMs.file.getFullPathName() == file.getFullPathName())
        {
            fileWithMs.timerefMs = timerefMilliseconds;
            juce::Logger::writeToLog("Updated timeref for " + file.getFileName() + ": " + juce::String(timerefMilliseconds));
            return;
        }
    }
}

int FileSelectorComponent::getTimerefForFile(const juce::File& file) const
{
    for (const auto& fileWithMs : selectedFiles)
    {
        if (fileWithMs.file.getFullPathName() == file.getFullPathName())
            return fileWithMs.timerefMs;
    }
    return 0;
}

juce::Array<juce::File> FileSelectorComponent::getSelectedFiles() const
{
    juce::Array<juce::File> files;
    for (const auto& fileWithMs : selectedFiles)
        files.add(fileWithMs.file);
    return files;
}

int FileSelectorComponent::getStartMillisecondsForFile(const juce::File& file) const
{
    for (const auto& fileWithMs : selectedFiles)
    {
        if (fileWithMs.file.getFullPathName() == file.getFullPathName())
            return fileWithMs.startMs;
    }
    return 0;
}

int FileSelectorComponent::getEndMillisecondsForFile(const juce::File& file) const
{
    for (const auto& fileWithMs : selectedFiles)
    {
        if (fileWithMs.file.getFullPathName() == file.getFullPathName())
            return fileWithMs.endMs;
    }
    return 0;
}

int FileSelectorComponent::getMillisecondsForFile(const juce::File& file) const
{
    return getStartMillisecondsForFile(file);
}

void FileSelectorComponent::clearSelection()
{
    auto filesToRemove = selectedFiles;
    for (const auto& fileWithMs : filesToRemove)
    {
        removeFileFromSelection(fileWithMs.file);
    }
}

void FileSelectorComponent::setRootFolder(const juce::File& folder)
{
    if (folder.isDirectory())
    {
        fileBrowser->setRoot(folder);
    }
}
