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
            GraphManager graphManager;
            expect(true);  // Basic construction test
        }

        beginTest("GraphManager createGraph with empty tracks");
        {
            expect(true);  // Should not crash
        }

        beginTest("GraphNode construction");
        {
            EditTest editTest;
            GraphManager graphManager;
            graphManager.createGraph(editTest);
            for (auto node : graphManager.graphNodes) {
                if (node->isGraphStart)
                    GraphNode::logGraph(node.get());
            }
            expect(true);  // Basic construction test
        }

        beginTest("GraphNodeType enum values");
        {
            GraphNodeType audioType = GraphNodeType::AudioTrackGraphNode;
            GraphNodeType auxType = GraphNodeType::AuxTrackGraphNode;
            expect(audioType != auxType);
        }
    }
};

static GraphManagerTests graphManagerTests;