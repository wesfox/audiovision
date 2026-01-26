#include "EditTest.h"

#include "Core/AudioClip/AudioClip.h"
#include "Core/Track/AudioTrack.h"
#include "Core/Track/AuxTrack.h"
#include "Core/Track/FolderTrack.h"
#include "Core/Track/Send.h"
#include "Core/Plugin/PluginRegistry.h"
#include "Utils/IO/EditSerializer.h"

// ------------------------ MainComponent Implementation ------------------------

EditTest::EditTest() {
    // Audio Files waves
    // auto voice1 = AudioFile::get(juce::File("/Users/nico/Music/440Hz 10s 48kHz.wav"));
    // auto voice2 = AudioFile::get(juce::File("/Users/nico/Music/120Hz 10s 48kHz.wav"));

    // Real AudioFiles
    auto voice1 = AudioFile::get(juce::File("/Users/nico/Music/ELYO PRINT_11.wav"));
    auto voice2 = AudioFile::get(juce::File("/Users/nico/Music/SEQ00_Actions horschampSDB.wav"));

    // Clips AudioTrack 1
    auto clipVoice11 = AudioClip::create(voice2, 48000 * 5, 48000 * 0, 48000 * 20);
    auto clipVoice12 = AudioClip::create(voice2, 48000 * 2, 48000 * 32, 48000 * 34);

    // AudioTrack 1
    auto audioTrack1 =  AudioTrack::create("audioTrack1");
    audioTrack1->addAudioClip(std::move(clipVoice11));
    audioTrack1->addAudioClip(std::move(clipVoice12));

    // Clips AudioTrack 2
    auto clipVoice21 = AudioClip::create(voice1, 48000 * 0, 48000 * 2, 48000 * 50);
    auto clipVoice22 = AudioClip::create(voice2, 48000 * 0, 48000 * 30, 48000 * 33);

    // AudioTrack 2
    auto audioTrack2 =  AudioTrack::create("audioTrack2");
    audioTrack2->addAudioClip(std::move(clipVoice21));
    audioTrack2->addAudioClip(std::move(clipVoice22));

    // Add the ReverbAuxTrack
    auto reverbAuxTrack = AuxTrack::create("reverbAuxTrack");
    PluginRegistry pluginRegistry;
    pluginRegistry.scan();
    if (auto reverbPlugin = pluginRegistry.getByName("TAL-Reverb-2")) {
        reverbAuxTrack->addPlugin(std::move(reverbPlugin));
    }

    auto send1toAux = Send::create(reverbAuxTrack);
    audioTrack1->addSend(std::move(send1toAux));

    auto send2toAux = Send::create(reverbAuxTrack);
    audioTrack2->addSend(std::move(send2toAux));


    auto DTrack = AudioTrack::create("DTrack");

    auto dialAux = FolderTrack::create(nullptr, "dialAux");
    dialAux->addTrack(audioTrack1);
    dialAux->addTrack(audioTrack2);
    audioTrack1->setOutput(dialAux); // make audioTrack1 output dialAux
    audioTrack2->setOutput(dialAux); // make audioTrack2 output dialAux
    reverbAuxTrack->setOutput(dialAux); // make reverbAuxTrack output dialAux

    dialAux->setOutput(DTrack);
    // set output
    setAudioOutputTrack(DTrack);
    // arm to record
    DTrack->setArmed(true);

    auto scene1 = std::make_shared<Scene>(0,48000*24, "Scene 1");
    auto scene2 = std::make_shared<Scene>(48000*24,48000*40, "Scene 2");
    addScene(std::move(scene1));
    addScene(std::move(scene2));

    // fill the constructed EditTest with these tracks that have already correct routing
    addTrack(std::move(reverbAuxTrack));
    addTrack(std::move(audioTrack1));
    addTrack(std::move(audioTrack2));
    addTrack(std::move(dialAux));
    addTrack(std::move(DTrack));
}
