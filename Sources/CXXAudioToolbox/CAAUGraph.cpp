//
// Copyright © 2021-2025 Stephen F. Booth
// Part of https://github.com/sbooth/CXXAudioToolbox
// MIT license
//

#import "CAAUGraph.hpp"
#import "AudioToolboxErrors.hpp"

CXXAudioToolbox::CAAUGraph::~CAAUGraph() noexcept
{
	reset();
}

CXXAudioToolbox::CAAUGraph::CAAUGraph(CAAUGraph&& other) noexcept
: graph_{other.release()}
{}

CXXAudioToolbox::CAAUGraph& CXXAudioToolbox::CAAUGraph::operator=(CAAUGraph&& other) noexcept
{
	reset(other.release());
	return *this;
}

void CXXAudioToolbox::CAAUGraph::New()
{
	Dispose();
	const auto result = NewAUGraph(&graph_);
	ThrowIfAUGraphError(result, "NewAUGraph");
}

void CXXAudioToolbox::CAAUGraph::Dispose()
{
	if(graph_) {
		const auto result = DisposeAUGraph(graph_);
		graph_ = nullptr;
		ThrowIfAUGraphError(result, "DisposeAUGraph");
	}
}

// MARK: - Node State

AUNode CXXAudioToolbox::CAAUGraph::AddNode(const AudioComponentDescription *inDescription)
{
	AUNode node{-1};
	const auto result = AUGraphAddNode(graph_, inDescription, &node);
	ThrowIfAUGraphError(result, "AUGraphAddNode");
	return node;
}

void CXXAudioToolbox::CAAUGraph::RemoveNode(AUNode inNode)
{
	const auto result = AUGraphRemoveNode(graph_, inNode);
	ThrowIfAUGraphError(result, "AUGraphRemoveNode");
}

UInt32 CXXAudioToolbox::CAAUGraph::GetNodeCount() const
{
	UInt32 numberOfNodes = 0;
	const auto result = AUGraphGetNodeCount(graph_, &numberOfNodes);
	ThrowIfAUGraphError(result, "AUGraphGetNodeCount");
	return numberOfNodes;
}

AUNode CXXAudioToolbox::CAAUGraph::GetIndNode(UInt32 inIndex) const
{
	AUNode node = -1;
	const auto result = AUGraphGetIndNode(graph_, inIndex, &node);
	ThrowIfAUGraphError(result, "AUGraphGetIndNode");
	return node;
}

void CXXAudioToolbox::CAAUGraph::NodeInfo(AUNode inNode, AudioComponentDescription *outDescription, AudioUnit *outAudioUnit) const
{
	const auto result = AUGraphNodeInfo(graph_, inNode, outDescription, outAudioUnit);
	ThrowIfAUGraphError(result, "AUGraphNodeInfo");
}


#if !TARGET_OS_IPHONE
// MARK: - Sub Graphs

AUNode CXXAudioToolbox::CAAUGraph::NewNodeSubGraph()
{
	AUNode node = -1;
	const auto result = AUGraphNewNodeSubGraph(graph_, &node);
	ThrowIfAUGraphError(result, "AUGraphNewNodeSubGraph");
	return node;
}

AUGraph CXXAudioToolbox::CAAUGraph::GetNodeInfoSubGraph(AUNode inNode) const
{
	AUGraph subGraph = nullptr;
	const auto result = AUGraphGetNodeInfoSubGraph(graph_, inNode, &subGraph);
	ThrowIfAUGraphError(result, "AUGraphGetNodeInfoSubGraph");
	return subGraph;
}

bool CXXAudioToolbox::CAAUGraph::IsNodeSubGraph(AUNode inNode) const
{
	Boolean flag = 0;
	const auto result = AUGraphIsNodeSubGraph(graph_, inNode, &flag);
	ThrowIfAUGraphError(result, "AUGraphIsNodeSubGraph");
	return flag != 0;
}
#endif /* !TARGET_OS_IPHONE */

// MARK: - Node Interactions

void CXXAudioToolbox::CAAUGraph::ConnectNodeInput(AUNode inSourceNode, UInt32 inSourceOutputNumber, AUNode inDestNode, UInt32 inDestInputNumber)
{
	const auto result = AUGraphConnectNodeInput(graph_, inSourceNode, inSourceOutputNumber, inDestNode, inDestInputNumber);
	ThrowIfAUGraphError(result, "AUGraphConnectNodeInput");
}

void CXXAudioToolbox::CAAUGraph::SetNodeInputCallback(AUNode inDestNode, UInt32 inDestInputNumber, const AURenderCallbackStruct *inInputCallback)
{
	const auto result = AUGraphSetNodeInputCallback(graph_, inDestNode, inDestInputNumber, inInputCallback);
	ThrowIfAUGraphError(result, "AUGraphSetNodeInputCallback");
}

void CXXAudioToolbox::CAAUGraph::DisconnectNodeInput(AUNode inDestNode, UInt32 inDestInputNumber)
{
	const auto result = AUGraphDisconnectNodeInput(graph_, inDestNode, inDestInputNumber);
	ThrowIfAUGraphError(result, "AUGraphDisconnectNodeInput");
}

void CXXAudioToolbox::CAAUGraph::ClearConnections()
{
	const auto result = AUGraphClearConnections(graph_);
	ThrowIfAUGraphError(result, "AUGraphClearConnections");
}

UInt32 CXXAudioToolbox::CAAUGraph::GetNumberOfInteractions() const
{
	UInt32 numberOfInteractions = 0;
	const auto result = AUGraphGetNumberOfInteractions(graph_, &numberOfInteractions);
	ThrowIfAUGraphError(result, "AUGraphGetNumberOfInteractions");
	return numberOfInteractions;
}

AUNodeInteraction CXXAudioToolbox::CAAUGraph::GetInteractionInfo(UInt32 inInteractionIndex) const
{
	AUNodeInteraction interaction{};
	const auto result = AUGraphGetInteractionInfo(graph_, inInteractionIndex, &interaction);
	ThrowIfAUGraphError(result, "AUGraphGetInteractionInfo");
	return interaction;
}

UInt32 CXXAudioToolbox::CAAUGraph::CountNodeInteractions(AUNode inNode) const
{
	UInt32 numberOfInteractions = 0;
	const auto result = AUGraphCountNodeInteractions(graph_, inNode, &numberOfInteractions);
	ThrowIfAUGraphError(result, "AUGraphCountNodeInteractions");
	return numberOfInteractions;
}

void CXXAudioToolbox::CAAUGraph::GetNodeInteractions(AUNode inNode, UInt32 *ioNumInteractions, AUNodeInteraction *outInteractions) const
{
	const auto result = AUGraphGetNodeInteractions(graph_, inNode, ioNumInteractions, outInteractions);
	ThrowIfAUGraphError(result, "AUGraphGetNodeInteractions");
}

// MARK: -

bool CXXAudioToolbox::CAAUGraph::Update()
{
	Boolean flag = 0;
	const auto result = AUGraphUpdate(graph_, &flag);
	ThrowIfAUGraphError(result, "AUGraphUpdate");
	return flag != 0;
}

// MARK: - State Management

void CXXAudioToolbox::CAAUGraph::Open()
{
	const auto result = AUGraphOpen(graph_);
	ThrowIfAUGraphError(result, "AUGraphOpen");
}

void CXXAudioToolbox::CAAUGraph::Close()
{
	const auto result = AUGraphClose(graph_);
	ThrowIfAUGraphError(result, "AUGraphClose");
}

void CXXAudioToolbox::CAAUGraph::Initialize()
{
	const auto result = AUGraphInitialize(graph_);
	ThrowIfAUGraphError(result, "AUGraphInitialize");
}

void CXXAudioToolbox::CAAUGraph::Uninitialize()
{
	const auto result = AUGraphUninitialize(graph_);
	ThrowIfAUGraphError(result, "AUGraphUninitialize");
}

void CXXAudioToolbox::CAAUGraph::Start()
{
	const auto result = AUGraphStart(graph_);
	ThrowIfAUGraphError(result, "AUGraphStart");
}

void CXXAudioToolbox::CAAUGraph::Stop()
{
	const auto result = AUGraphStop(graph_);
	ThrowIfAUGraphError(result, "AUGraphStop");
}

bool CXXAudioToolbox::CAAUGraph::IsOpen() const
{
	Boolean flag = 0;
	const auto result = AUGraphIsOpen(graph_, &flag);
	ThrowIfAUGraphError(result, "AUGraphIsOpen");
	return flag != 0;
}

bool CXXAudioToolbox::CAAUGraph::IsInitialized() const
{
	Boolean flag = 0;
	const auto result = AUGraphIsInitialized(graph_, &flag);
	ThrowIfAUGraphError(result, "AUGraphIsInitialized");
	return flag != 0;
}

bool CXXAudioToolbox::CAAUGraph::IsRunning() const
{
	Boolean flag = 0;
	const auto result = AUGraphIsRunning(graph_, &flag);
	ThrowIfAUGraphError(result, "AUGraphIsRunning");
	return flag != 0;
}

// MARK: - Utilities

Float32 CXXAudioToolbox::CAAUGraph::GetCPULoad() const
{
	Float32 value = 0;
	const auto result = AUGraphGetCPULoad(graph_, &value);
	ThrowIfAUGraphError(result, "AUGraphGetCPULoad");
	return value;
}

Float32 CXXAudioToolbox::CAAUGraph::GetMaxCPULoad() const
{
	Float32 value = 0;
	const auto result = AUGraphGetMaxCPULoad(graph_, &value);
	ThrowIfAUGraphError(result, "AUGraphGetMaxCPULoad");
	return value;
}

void CXXAudioToolbox::CAAUGraph::AddRenderNotify(AURenderCallback inCallback, void *inRefCon)
{
	const auto result = AUGraphAddRenderNotify(graph_, inCallback, inRefCon);
	ThrowIfAUGraphError(result, "AUGraphAddRenderNotify");
}

void CXXAudioToolbox::CAAUGraph::RemoveRenderNotify(AURenderCallback inCallback, void *inRefCon)
{
	const auto result = AUGraphRemoveRenderNotify(graph_, inCallback, inRefCon);
	ThrowIfAUGraphError(result, "AUGraphRemoveRenderNotify");
}

// MARK: - Helpers

std::vector<AUNode> CXXAudioToolbox::CAAUGraph::Nodes() const
{
	auto nodeCount = GetNodeCount();
	auto nodes = std::vector<AUNode>(nodeCount);
	for(UInt32 i = 0; i < nodeCount; ++i) {
		auto node = GetIndNode(i);
		nodes.push_back(node);
	}
	return nodes;
}

std::vector<AUNodeInteraction> CXXAudioToolbox::CAAUGraph::NodeInteractions(AUNode inNode) const
{
	auto interactionCount = CountNodeInteractions(inNode);
	auto interactions = std::vector<AUNodeInteraction>(interactionCount);
	GetNodeInteractions(inNode, &interactionCount, interactions.data());
	return interactions;
}

std::map<AUNode, std::vector<AUNodeInteraction>> CXXAudioToolbox::CAAUGraph::NodesAndInteractions() const
{
	auto nodes = Nodes();
	auto nodesAndInteractions = std::map<AUNode, std::vector<AUNodeInteraction>>();
	for(auto node : nodes)
		nodesAndInteractions[node] = NodeInteractions(node);
	return nodesAndInteractions;
}

Float64 CXXAudioToolbox::CAAUGraph::Latency() const
{
	Float64 latency = 0;
	auto nodeCount = GetNodeCount();
	for(UInt32 i = 0; i < nodeCount; ++i) {
		auto node = GetIndNode(i);
		AudioUnit au = nullptr;
		NodeInfo(node, nullptr, &au);

		Float64 auLatency = 0;
		UInt32 dataSize = sizeof auLatency;
		const auto result = AudioUnitGetProperty(au, kAudioUnitProperty_Latency, kAudioUnitScope_Global, 0, &auLatency, &dataSize);
		ThrowIfAudioUnitError(result, "AudioUnitGetProperty (kAudioUnitProperty_Latency, kAudioUnitScope_Global)");

		latency += auLatency;
	}

	return latency;
}

Float64 CXXAudioToolbox::CAAUGraph::TailTime() const
{
	Float64 tailTime = 0;
	auto nodeCount = GetNodeCount();
	for(UInt32 i = 0; i < nodeCount; ++i) {
		auto node = GetIndNode(i);
		AudioUnit au = nullptr;
		NodeInfo(node, nullptr, &au);

		Float64 auTailTime = 0;
		UInt32 dataSize = sizeof auTailTime;
		const auto result = AudioUnitGetProperty(au, kAudioUnitProperty_TailTime, kAudioUnitScope_Global, 0, &auTailTime, &dataSize);
		ThrowIfAudioUnitError(result, "AudioUnitGetProperty (kAudioUnitProperty_TailTime, kAudioUnitScope_Global)");

		tailTime += auTailTime;
	}

	return tailTime;
}
