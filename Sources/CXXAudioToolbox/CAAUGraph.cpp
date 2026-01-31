//
// SPDX-FileCopyrightText: 2021 Stephen F. Booth <contact@sbooth.dev>
// SPDX-License-Identifier: MIT
//
// Part of https://github.com/sbooth/CXXAudioToolbox
//

#include "audio_toolbox/CAAUGraph.hpp"

#include "AudioToolboxErrors.hpp"

audio_toolbox::CAAUGraph::~CAAUGraph() noexcept { reset(); }

audio_toolbox::CAAUGraph::CAAUGraph(CAAUGraph &&other) noexcept : graph_{other.release()} {}

audio_toolbox::CAAUGraph &audio_toolbox::CAAUGraph::operator=(CAAUGraph &&other) noexcept {
    reset(other.release());
    return *this;
}

void audio_toolbox::CAAUGraph::New() {
    Dispose();
    const auto result = NewAUGraph(&graph_);
    ThrowIfAUGraphError(result, "NewAUGraph");
}

void audio_toolbox::CAAUGraph::Dispose() {
    if (graph_) {
        const auto result = DisposeAUGraph(graph_);
        graph_ = nullptr;
        ThrowIfAUGraphError(result, "DisposeAUGraph");
    }
}

// MARK: - Node State

AUNode audio_toolbox::CAAUGraph::AddNode(const AudioComponentDescription *inDescription) {
    AUNode node{-1};
    const auto result = AUGraphAddNode(graph_, inDescription, &node);
    ThrowIfAUGraphError(result, "AUGraphAddNode");
    return node;
}

void audio_toolbox::CAAUGraph::RemoveNode(AUNode inNode) {
    const auto result = AUGraphRemoveNode(graph_, inNode);
    ThrowIfAUGraphError(result, "AUGraphRemoveNode");
}

UInt32 audio_toolbox::CAAUGraph::GetNodeCount() const {
    UInt32 numberOfNodes = 0;
    const auto result = AUGraphGetNodeCount(graph_, &numberOfNodes);
    ThrowIfAUGraphError(result, "AUGraphGetNodeCount");
    return numberOfNodes;
}

AUNode audio_toolbox::CAAUGraph::GetIndNode(UInt32 inIndex) const {
    AUNode node = -1;
    const auto result = AUGraphGetIndNode(graph_, inIndex, &node);
    ThrowIfAUGraphError(result, "AUGraphGetIndNode");
    return node;
}

void audio_toolbox::CAAUGraph::NodeInfo(AUNode inNode, AudioComponentDescription *outDescription,
                                        AudioUnit *outAudioUnit) const {
    const auto result = AUGraphNodeInfo(graph_, inNode, outDescription, outAudioUnit);
    ThrowIfAUGraphError(result, "AUGraphNodeInfo");
}

#if !TARGET_OS_IPHONE
// MARK: - Sub Graphs

AUNode audio_toolbox::CAAUGraph::NewNodeSubGraph() {
    AUNode node = -1;
    const auto result = AUGraphNewNodeSubGraph(graph_, &node);
    ThrowIfAUGraphError(result, "AUGraphNewNodeSubGraph");
    return node;
}

AUGraph audio_toolbox::CAAUGraph::GetNodeInfoSubGraph(AUNode inNode) const {
    AUGraph subGraph = nullptr;
    const auto result = AUGraphGetNodeInfoSubGraph(graph_, inNode, &subGraph);
    ThrowIfAUGraphError(result, "AUGraphGetNodeInfoSubGraph");
    return subGraph;
}

bool audio_toolbox::CAAUGraph::IsNodeSubGraph(AUNode inNode) const {
    Boolean flag = 0;
    const auto result = AUGraphIsNodeSubGraph(graph_, inNode, &flag);
    ThrowIfAUGraphError(result, "AUGraphIsNodeSubGraph");
    return flag != 0;
}
#endif /* !TARGET_OS_IPHONE */

// MARK: - Node Interactions

void audio_toolbox::CAAUGraph::ConnectNodeInput(AUNode inSourceNode, UInt32 inSourceOutputNumber, AUNode inDestNode,
                                                UInt32 inDestInputNumber) {
    const auto result =
            AUGraphConnectNodeInput(graph_, inSourceNode, inSourceOutputNumber, inDestNode, inDestInputNumber);
    ThrowIfAUGraphError(result, "AUGraphConnectNodeInput");
}

void audio_toolbox::CAAUGraph::SetNodeInputCallback(AUNode inDestNode, UInt32 inDestInputNumber,
                                                    const AURenderCallbackStruct *inInputCallback) {
    const auto result = AUGraphSetNodeInputCallback(graph_, inDestNode, inDestInputNumber, inInputCallback);
    ThrowIfAUGraphError(result, "AUGraphSetNodeInputCallback");
}

void audio_toolbox::CAAUGraph::DisconnectNodeInput(AUNode inDestNode, UInt32 inDestInputNumber) {
    const auto result = AUGraphDisconnectNodeInput(graph_, inDestNode, inDestInputNumber);
    ThrowIfAUGraphError(result, "AUGraphDisconnectNodeInput");
}

void audio_toolbox::CAAUGraph::ClearConnections() {
    const auto result = AUGraphClearConnections(graph_);
    ThrowIfAUGraphError(result, "AUGraphClearConnections");
}

UInt32 audio_toolbox::CAAUGraph::GetNumberOfInteractions() const {
    UInt32 numberOfInteractions = 0;
    const auto result = AUGraphGetNumberOfInteractions(graph_, &numberOfInteractions);
    ThrowIfAUGraphError(result, "AUGraphGetNumberOfInteractions");
    return numberOfInteractions;
}

AUNodeInteraction audio_toolbox::CAAUGraph::GetInteractionInfo(UInt32 inInteractionIndex) const {
    AUNodeInteraction interaction{};
    const auto result = AUGraphGetInteractionInfo(graph_, inInteractionIndex, &interaction);
    ThrowIfAUGraphError(result, "AUGraphGetInteractionInfo");
    return interaction;
}

UInt32 audio_toolbox::CAAUGraph::CountNodeInteractions(AUNode inNode) const {
    UInt32 numberOfInteractions = 0;
    const auto result = AUGraphCountNodeInteractions(graph_, inNode, &numberOfInteractions);
    ThrowIfAUGraphError(result, "AUGraphCountNodeInteractions");
    return numberOfInteractions;
}

void audio_toolbox::CAAUGraph::GetNodeInteractions(AUNode inNode, UInt32 *ioNumInteractions,
                                                   AUNodeInteraction *outInteractions) const {
    const auto result = AUGraphGetNodeInteractions(graph_, inNode, ioNumInteractions, outInteractions);
    ThrowIfAUGraphError(result, "AUGraphGetNodeInteractions");
}

// MARK: -

bool audio_toolbox::CAAUGraph::Update() {
    Boolean flag = 0;
    const auto result = AUGraphUpdate(graph_, &flag);
    ThrowIfAUGraphError(result, "AUGraphUpdate");
    return flag != 0;
}

// MARK: - State Management

void audio_toolbox::CAAUGraph::Open() {
    const auto result = AUGraphOpen(graph_);
    ThrowIfAUGraphError(result, "AUGraphOpen");
}

void audio_toolbox::CAAUGraph::Close() {
    const auto result = AUGraphClose(graph_);
    ThrowIfAUGraphError(result, "AUGraphClose");
}

void audio_toolbox::CAAUGraph::Initialize() {
    const auto result = AUGraphInitialize(graph_);
    ThrowIfAUGraphError(result, "AUGraphInitialize");
}

void audio_toolbox::CAAUGraph::Uninitialize() {
    const auto result = AUGraphUninitialize(graph_);
    ThrowIfAUGraphError(result, "AUGraphUninitialize");
}

void audio_toolbox::CAAUGraph::Start() {
    const auto result = AUGraphStart(graph_);
    ThrowIfAUGraphError(result, "AUGraphStart");
}

void audio_toolbox::CAAUGraph::Stop() {
    const auto result = AUGraphStop(graph_);
    ThrowIfAUGraphError(result, "AUGraphStop");
}

bool audio_toolbox::CAAUGraph::IsOpen() const {
    Boolean flag = 0;
    const auto result = AUGraphIsOpen(graph_, &flag);
    ThrowIfAUGraphError(result, "AUGraphIsOpen");
    return flag != 0;
}

bool audio_toolbox::CAAUGraph::IsInitialized() const {
    Boolean flag = 0;
    const auto result = AUGraphIsInitialized(graph_, &flag);
    ThrowIfAUGraphError(result, "AUGraphIsInitialized");
    return flag != 0;
}

bool audio_toolbox::CAAUGraph::IsRunning() const {
    Boolean flag = 0;
    const auto result = AUGraphIsRunning(graph_, &flag);
    ThrowIfAUGraphError(result, "AUGraphIsRunning");
    return flag != 0;
}

// MARK: - Utilities

Float32 audio_toolbox::CAAUGraph::GetCPULoad() const {
    Float32 value = 0;
    const auto result = AUGraphGetCPULoad(graph_, &value);
    ThrowIfAUGraphError(result, "AUGraphGetCPULoad");
    return value;
}

Float32 audio_toolbox::CAAUGraph::GetMaxCPULoad() const {
    Float32 value = 0;
    const auto result = AUGraphGetMaxCPULoad(graph_, &value);
    ThrowIfAUGraphError(result, "AUGraphGetMaxCPULoad");
    return value;
}

void audio_toolbox::CAAUGraph::AddRenderNotify(AURenderCallback inCallback, void *inRefCon) {
    const auto result = AUGraphAddRenderNotify(graph_, inCallback, inRefCon);
    ThrowIfAUGraphError(result, "AUGraphAddRenderNotify");
}

void audio_toolbox::CAAUGraph::RemoveRenderNotify(AURenderCallback inCallback, void *inRefCon) {
    const auto result = AUGraphRemoveRenderNotify(graph_, inCallback, inRefCon);
    ThrowIfAUGraphError(result, "AUGraphRemoveRenderNotify");
}

// MARK: - Helpers

std::vector<AUNode> audio_toolbox::CAAUGraph::Nodes() const {
    auto nodeCount = GetNodeCount();
    auto nodes = std::vector<AUNode>(nodeCount);
    for (UInt32 i = 0; i < nodeCount; ++i) {
        auto node = GetIndNode(i);
        nodes.push_back(node);
    }
    return nodes;
}

std::vector<AUNodeInteraction> audio_toolbox::CAAUGraph::NodeInteractions(AUNode inNode) const {
    auto interactionCount = CountNodeInteractions(inNode);
    auto interactions = std::vector<AUNodeInteraction>(interactionCount);
    GetNodeInteractions(inNode, &interactionCount, interactions.data());
    return interactions;
}

std::map<AUNode, std::vector<AUNodeInteraction>> audio_toolbox::CAAUGraph::NodesAndInteractions() const {
    auto nodes = Nodes();
    auto nodesAndInteractions = std::map<AUNode, std::vector<AUNodeInteraction>>();
    for (auto node : nodes) {
        nodesAndInteractions[node] = NodeInteractions(node);
    }
    return nodesAndInteractions;
}

Float64 audio_toolbox::CAAUGraph::Latency() const {
    Float64 latency = 0;
    auto nodeCount = GetNodeCount();
    for (UInt32 i = 0; i < nodeCount; ++i) {
        auto node = GetIndNode(i);
        AudioUnit au = nullptr;
        NodeInfo(node, nullptr, &au);

        Float64 auLatency = 0;
        UInt32 dataSize = sizeof auLatency;
        const auto result =
                AudioUnitGetProperty(au, kAudioUnitProperty_Latency, kAudioUnitScope_Global, 0, &auLatency, &dataSize);
        ThrowIfAudioUnitError(result, "AudioUnitGetProperty (kAudioUnitProperty_Latency, kAudioUnitScope_Global)");

        latency += auLatency;
    }

    return latency;
}

Float64 audio_toolbox::CAAUGraph::TailTime() const {
    Float64 tailTime = 0;
    auto nodeCount = GetNodeCount();
    for (UInt32 i = 0; i < nodeCount; ++i) {
        auto node = GetIndNode(i);
        AudioUnit au = nullptr;
        NodeInfo(node, nullptr, &au);

        Float64 auTailTime = 0;
        UInt32 dataSize = sizeof auTailTime;
        const auto result = AudioUnitGetProperty(au, kAudioUnitProperty_TailTime, kAudioUnitScope_Global, 0,
                                                 &auTailTime, &dataSize);
        ThrowIfAudioUnitError(result, "AudioUnitGetProperty (kAudioUnitProperty_TailTime, kAudioUnitScope_Global)");

        tailTime += auTailTime;
    }

    return tailTime;
}
