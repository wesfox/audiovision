#include "MainComponent.h"

#include "AudioEngine/AudioEngine.h"
#include "Core/CustomEdits/EditTest.h"
#include "Core/CustomEdits/ImportEdit.h"
#include "Utils/IO/EditSerializer.h"
#include "AudioEngine/Parameters/ValueTreeManager.h"
#include "AudioEngine/Parameters/ParameterFactory.h"

//==============================================================================
MainComponent::MainComponent()
{
    setSize (600, 400);

    edit = ImportEdit::importEdit();

    fileSelector = std::make_unique<FileSelectorComponent>();
    fileSelector->onPlaySession = [this](const FileSelectorComponent::ImportedTrack& track) {
        audioEngine->transport->rewind();
        audioEngine->transport->play();
    };
    addAndMakeVisible(fileSelector.get());

    recordComponent = std::make_unique<RecordComponent>();
    recordComponent->onStartRecording = [this]() {
        if (audioEngine != nullptr) {
            audioEngine->startRecording();
        }
    };
    recordComponent->onStopRecording = [this]() {
        if (audioEngine != nullptr) {
            audioEngine->stopRecording();
        }
    };
    addAndMakeVisible(recordComponent.get());

    pluginEditorWindow = std::make_unique<PluginEditorWindow>();

    // Test sound stuff
    audioEngine = std::make_unique<AudioEngine>(edit);
    audioEngine->start();

    // end test sound stuff
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(1.0);
    volumeSlider.onValueChange = [this]() {
        if (volumeParam != nullptr) {
            if (audioEngine != nullptr && !audioEngine->getGraphInstances().empty() && edit != nullptr) {
                auto& graphManager = audioEngine->getGraphInstances().front()->getGraphManager();
                auto& valueTreeManager = graphManager.getValueTreeManager();
                const auto& tracks = edit->getTracks();
                for (auto track : tracks) {
                    if (track->getName() == "DTrack") {
                        valueTreeManager.setParameterValue(track->getId(), ParameterKey::Volume,
                                                           static_cast<float>(volumeSlider.getValue()));
                        break;
                    }
                }
            }
        }
    };
    addAndMakeVisible(volumeSlider);

    stereoPanSlider.setRange(-1.0, 1.0, 0.01);
    stereoPanSlider.setValue(0.0);
    stereoPanSlider.onValueChange = [this]() {
        if (stereoPanParam != nullptr) {
            if (audioEngine != nullptr && !audioEngine->getGraphInstances().empty() && edit != nullptr) {
                auto& graphManager = audioEngine->getGraphInstances().front()->getGraphManager();
                auto& valueTreeManager = graphManager.getValueTreeManager();
                const auto& tracks = edit->getTracks();
                for (auto track : tracks) {
                    if (track->getName() == "DTrack") {
                        valueTreeManager.setParameterValue(track->getId(), ParameterKey::StereoPan,
                                                           static_cast<float>(stereoPanSlider.getValue()));
                        break;
                    }
                }
            }
        }
    };
    addAndMakeVisible(stereoPanSlider);

    if (audioEngine != nullptr && !audioEngine->getGraphInstances().empty() && edit != nullptr) {
        auto& graphManager = audioEngine->getGraphInstances().front()->getGraphManager();
        auto& valueTreeManager = graphManager.getValueTreeManager();
        if (auto* apvts = valueTreeManager.getState()) {
            const auto& tracks = edit->getTracks();
            if (!tracks.empty()) {
                for (auto track : tracks) {
                    if (track->getName() == "DTrack"){
                        juce::Logger::writeToLog("slider attached to DTrack");
                        const auto volumeParamId = valueTreeManager.makeParamId(track->getId(), ParameterKey::Volume);
                        volumeParam = apvts->getParameter(volumeParamId);
                        if (volumeParam != nullptr) {
                            volumeSlider.setValue(
                                valueTreeManager.getParameterValue(track->getId(), ParameterKey::Volume),
                                juce::dontSendNotification);
                        }

                        const auto panParamId = valueTreeManager.makeParamId(track->getId(), ParameterKey::StereoPan);
                        stereoPanParam = apvts->getParameter(panParamId);
                        if (stereoPanParam != nullptr) {
                            stereoPanSlider.setValue(
                                valueTreeManager.getParameterValue(track->getId(), ParameterKey::StereoPan),
                                juce::dontSendNotification);
                        }
                    }
                }
            }

        }
    }

    setSize(800, 700);
}

MainComponent::~MainComponent()
{
    shutdown();
}

void MainComponent::shutdown()
{
    EditSerializer::exportToFile(*edit, juce::File("/Users/nico/code/Audiovision/exports"));
    if (isShutDown) {
        return;
    }
    isShutDown = true;
    if (pluginEditorWindow != nullptr) {
        pluginEditorWindow->setPluginNode(nullptr);
        pluginEditorWindow->setVisible(false);
    }
    pluginEditorWindow.reset();
    recordComponent.reset();
    if (audioEngine != nullptr) {
        audioEngine->shutdown();
    }
    audioEngine.reset();
    if (auto* messageManager = juce::MessageManager::getInstanceWithoutCreating()) {
        messageManager->runDispatchLoopUntil(50);
    }
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();
    auto controlsArea = bounds.removeFromTop(40);
    auto sliderArea = bounds.removeFromTop(40);
    if (recordComponent != nullptr) {
        recordComponent->setBounds(controlsArea);
    }
    volumeSlider.setBounds(sliderArea.removeFromLeft(200).reduced(8, 4));
    stereoPanSlider.setBounds(sliderArea.removeFromLeft(200).reduced(8, 4));
    if (fileSelector != nullptr) {
        fileSelector->setBounds(bounds);
    }
}
