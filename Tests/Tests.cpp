//
// Created by Nicolas on 15/01/2026.
//
#include <JuceHeader.h>

int main()
{
    juce::UnitTestRunner runner;
    runner.setPassesAreLogged(false);
    runner.runAllTests();
    auto failures = 0;
    for (int i = 0; i < runner.getNumResults(); i++) {
        failures+=runner.getResult(i)->failures;
        if (failures == 0)
            continue;

        for (auto message : runner.getResult(i)->messages) {
            juce::Logger::writeToLog(message);
        }

    }
    if (failures > 0)
        return EXIT_FAILURE;

    juce::Logger::writeToLog("========================================");
    juce::Logger::writeToLog("Yay, tests runned without any failures !");
    return 0;
}