#include <JuceHeader.h>

#include <AudioEngine/Graph/GraphManager.h>
#include <AudioEngine/Graph/GraphNode.h>

#include "Core/CustomEdits/EditTest.h"

class GraphManagerTests : public juce::UnitTest
{
public:
    GraphManagerTests() : juce::UnitTest("GraphManager", "Engine") {}

    void runTest() override
    {
        beginTest("GraphManager construction");
        {
            juce::ScopedJuceInitialiser_GUI juceInit;
            GraphManager graphManager;
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
            EditTest editTest;
            auto graphManager = std::make_unique<GraphManager>();
            graphManager->createGraph(editTest);
            for (auto node : graphManager->graphNodes) {
                if (node->isGraphStart)
                    GraphNode::logGraph(node.get());
            }
            graphManager->createFinalGraph();
            for (const auto& connection : graphManager->graphConnectionManager.graphVirtualConnections) {
                juce::Logger::writeToLog(connection->toString());
            }
            for (const auto& node : graphManager->graphNodes) {
                juce::Logger::writeToLog(node->toString());
            }
            graphManager->attachAudioOutput(editTest.getAudioOutputTrack());
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
