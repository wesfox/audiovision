#include "EditTest.h"

#include "Core/AudioClip/AudioClip.h"
#include "Core/Track/AudioTrack.h"
#include "Core/Track/AuxTrack.h"
#include "Core/Track/FolderTrack.h"
#include "Core/Track/Send.h"

// ------------------------ MainComponent Implementation ------------------------

EditTest::EditTest() {
    // Audio Files waves
    auto voice1 = AudioFile::get(juce::File("/Users/nico/Music/440Hz 10s 48kHz.wav"));
    auto voice2 = AudioFile::get(juce::File("/Users/nico/Music/120Hz 10s 48kHz.wav"));

    // Real AudioFiles
    // auto voice1 = AudioFile::get(juce::File("/Users/nico/Music/SEQ07_RespirationsCleo.wav"));
    // auto voice2 = AudioFile::get(juce::File("/Users/nico/Music/SEQ00_Actions horschampSDB.wav"));

    // Clips AudioTrack 1
    auto clipVoice11 = AudioClip::create(voice1, 48000 * 0, 48000 * 0, 48000 * 2);
    auto clipVoice12 = AudioClip::create(voice2, 48000 * 2, 48000 * 32, 48000 * 34);

    // AudioTrack 1
    auto audioTrack1 =  AudioTrack::create("audioTrack1");
    audioTrack1->addAudioClip(std::move(clipVoice11));
    audioTrack1->addAudioClip(std::move(clipVoice12));

    // Clips AudioTrack 2
    auto clipVoice21 = AudioClip::create(voice1, 48000 * 2, 48000 * 2, 48000 * 5);
    auto clipVoice22 = AudioClip::create(voice2, 48000 * 0, 48000 * 30, 48000 * 33);

    // AudioTrack 2
    auto audioTrack2 =  AudioTrack::create("audioTrack2");
    audioTrack2->addAudioClip(std::move(clipVoice21));
    audioTrack2->addAudioClip(std::move(clipVoice22));

    // Add the ReverbAuxTrack
    auto reverbAuxTrack = AuxTrack::create("reverbAuxTrack");

    auto send1toAux = Send::create(reverbAuxTrack);
    audioTrack1->addSend(std::move(send1toAux));

    auto send2toAux = Send::create(reverbAuxTrack);
    audioTrack2->addSend(std::move(send2toAux));

    // Set audioTracks output to the dialAux (which is a folder)
    auto dialAux = FolderTrack::create(nullptr, "dialAux");
    dialAux->addTrack(audioTrack1); // make audioTrack1 output dialAux
    dialAux->addTrack(audioTrack2); // make audioTrack1 output dialAux
    audioTrack1->setOutput(dialAux);
    audioTrack2->setOutput(dialAux);
    reverbAuxTrack->setOutput(dialAux);

    // Make dialAux output to DTrack
    auto DTrack = AuxTrack::create("DTrack");
    dialAux->setOutput(DTrack);

    // set output
    setAudioOutputTrack(DTrack);

    // fill the constructed EditTest with these tracks that have already correct routing
    addTrack(std::move(audioTrack1));
    addTrack(std::move(audioTrack2));
    addTrack(std::move(dialAux));
    addTrack(std::move(DTrack));
    addTrack(std::move(reverbAuxTrack));
}
