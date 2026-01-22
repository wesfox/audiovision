#include <JuceHeader.h>

#include <AudioEngine/AudioEngine.h>
#include <AudioEngine/Graph/Model/GraphNode.h>
#include <Utils/IO/GraphMermaidExporter.h>
#include <Utils/IO/AudioGraphMermaidExporter.h>

#include "Core/CustomEdits/EditTest.h"
#include "Utils/IO/EditSerializer.h"

class GraphManagerTests : public juce::UnitTest
{
public:
    GraphManagerTests() : juce::UnitTest("GraphManager", "Engine") {}

    void runTest() override
    {
        beginTest("GraphManager construction");
        {
            juce::ScopedJuceInitialiser_GUI juceInit;
            auto edit = std::make_shared<EditTest>();
            AudioEngine audioEngine(edit);
            expect(true);  // Basic construction test
        }

        beginTest("GraphManager createGraph with empty tracks");
        {
            juce::ScopedJuceInitialiser_GUI juceInit;
            expect(true);  // Should not crash
        }

        beginTest("GraphNode construction");
        {
            juce::ScopedJuceInitialiser_GUI juceInit;
            auto edit = std::make_shared<EditTest>();
            AudioEngine audioEngine(edit);

            for (auto& graphInstance : audioEngine.getGraphInstances()) {
                auto& graphManager = graphInstance->getGraphManager();;
                auto mermaid = GraphMermaidExporter::exportMermaid(graphManager.getGraphDescription());
                juce::Logger::writeToLog(mermaid);
                auto audioGraphMermaid = AudioGraphMermaidExporter::exportMermaid(graphInstance->getGraph());
                juce::Logger::writeToLog(audioGraphMermaid);

            }
            EditSerializer::exportToFile(*edit.get(), File("/Users/nico/code/Audiovision/exports"));
            expect(true);  // Basic construction test
        }

        beginTest("GraphNodeType enum values");
        {
            juce::ScopedJuceInitialiser_GUI juceInit;
            GraphNodeType audioType = GraphNodeType::AudioTrackGraphNode;
            GraphNodeType auxType = GraphNodeType::AuxTrackGraphNode;
            expect(audioType != auxType);
        }
    }
};

static GraphManagerTests graphManagerTests;
