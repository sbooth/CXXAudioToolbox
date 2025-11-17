//
// Copyright © 2021-2025 Stephen F. Booth
// Part of https://github.com/sbooth/CXXAudioToolbox
// MIT license
//

#import <utility>

#import "ATAUGraph.hpp"
#import "ATErrors.hpp"

CXXAudioToolbox::ATAUGraph::~ATAUGraph() noexcept
{
	if(auGraph_)
		DisposeAUGraph(auGraph_);
}

CXXAudioToolbox::ATAUGraph::ATAUGraph(ATAUGraph&& rhs) noexcept
: auGraph_{std::exchange(rhs.auGraph_, nullptr)}
{}

CXXAudioToolbox::ATAUGraph& CXXAudioToolbox::ATAUGraph::operator=(ATAUGraph&& rhs) noexcept
{
	if(this != &rhs) {
		if(auGraph_)
			DisposeAUGraph(auGraph_);
		auGraph_ = std::exchange(rhs.auGraph_, nullptr);
	}
	return *this;
}

void CXXAudioToolbox::ATAUGraph::New()
{
	Dispose();
	auto result = NewAUGraph(&auGraph_);
	ThrowIfAUGraphError(result, "NewAUGraph");
}

void CXXAudioToolbox::ATAUGraph::Dispose()
{
	if(auGraph_) {
		auto result = DisposeAUGraph(auGraph_);
		auGraph_ = nullptr;
		ThrowIfAUGraphError(result, "DisposeAUGraph");
	}
}

// MARK: - Node State

AUNode CXXAudioToolbox::ATAUGraph::AddNode(const AudioComponentDescription *inDescription)
{
	AUNode node{-1};
	auto result = AUGraphAddNode(auGraph_, inDescription, &node);
	ThrowIfAUGraphError(result, "AUGraphAddNode");
	return node;
}

void CXXAudioToolbox::ATAUGraph::RemoveNode(AUNode inNode)
{
	auto result = AUGraphRemoveNode(auGraph_, inNode);
	ThrowIfAUGraphError(result, "AUGraphRemoveNode");
}

UInt32 CXXAudioToolbox::ATAUGraph::GetNodeCount() const
{
	UInt32 numberOfNodes = 0;
	auto result = AUGraphGetNodeCount(auGraph_, &numberOfNodes);
	ThrowIfAUGraphError(result, "AUGraphGetNodeCount");
	return numberOfNodes;
}

AUNode CXXAudioToolbox::ATAUGraph::GetIndNode(UInt32 inIndex) const
{
	AUNode node = -1;
	auto result = AUGraphGetIndNode(auGraph_, inIndex, &node);
	ThrowIfAUGraphError(result, "AUGraphGetIndNode");
	return node;
}

void CXXAudioToolbox::ATAUGraph::NodeInfo(AUNode inNode, AudioComponentDescription *outDescription, AudioUnit *outAudioUnit) const
{
	auto result = AUGraphNodeInfo(auGraph_, inNode, outDescription, outAudioUnit);
	ThrowIfAUGraphError(result, "AUGraphNodeInfo");
}


#if !TARGET_OS_IPHONE
// MARK: - Sub Graphs

AUNode CXXAudioToolbox::ATAUGraph::NewNodeSubGraph()
{
	AUNode node = -1;
	auto result = AUGraphNewNodeSubGraph(auGraph_, &node);
	ThrowIfAUGraphError(result, "AUGraphNewNodeSubGraph");
	return node;
}

AUGraph CXXAudioToolbox::ATAUGraph::GetNodeInfoSubGraph(AUNode inNode) const
{
	AUGraph subGraph = nullptr;
	auto result = AUGraphGetNodeInfoSubGraph(auGraph_, inNode, &subGraph);
	ThrowIfAUGraphError(result, "AUGraphGetNodeInfoSubGraph");
	return subGraph;
}

bool CXXAudioToolbox::ATAUGraph::IsNodeSubGraph(AUNode inNode) const
{
	Boolean flag = 0;
	auto result = AUGraphIsNodeSubGraph(auGraph_, inNode, &flag);
	ThrowIfAUGraphError(result, "AUGraphIsNodeSubGraph");
	return flag != 0;
}
#endif /* !TARGET_OS_IPHONE */

// MARK: - Node Interactions

void CXXAudioToolbox::ATAUGraph::ConnectNodeInput(AUNode inSourceNode, UInt32 inSourceOutputNumber, AUNode inDestNode, UInt32 inDestInputNumber)
{
	auto result = AUGraphConnectNodeInput(auGraph_, inSourceNode, inSourceOutputNumber, inDestNode, inDestInputNumber);
	ThrowIfAUGraphError(result, "AUGraphConnectNodeInput");
}

void CXXAudioToolbox::ATAUGraph::SetNodeInputCallback(AUNode inDestNode, UInt32 inDestInputNumber, const AURenderCallbackStruct *inInputCallback)
{
	auto result = AUGraphSetNodeInputCallback(auGraph_, inDestNode, inDestInputNumber, inInputCallback);
	ThrowIfAUGraphError(result, "AUGraphSetNodeInputCallback");
}

void CXXAudioToolbox::ATAUGraph::DisconnectNodeInput(AUNode inDestNode, UInt32 inDestInputNumber)
{
	auto result = AUGraphDisconnectNodeInput(auGraph_, inDestNode, inDestInputNumber);
	ThrowIfAUGraphError(result, "AUGraphDisconnectNodeInput");
}

void CXXAudioToolbox::ATAUGraph::ClearConnections()
{
	auto result = AUGraphClearConnections(auGraph_);
	ThrowIfAUGraphError(result, "AUGraphClearConnections");
}

UInt32 CXXAudioToolbox::ATAUGraph::GetNumberOfInteractions() const
{
	UInt32 numberOfInteractions = 0;
	auto result = AUGraphGetNumberOfInteractions(auGraph_, &numberOfInteractions);
	ThrowIfAUGraphError(result, "AUGraphGetNumberOfInteractions");
	return numberOfInteractions;
}

AUNodeInteraction CXXAudioToolbox::ATAUGraph::GetInteractionInfo(UInt32 inInteractionIndex) const
{
	AUNodeInteraction interaction{};
	auto result = AUGraphGetInteractionInfo(auGraph_, inInteractionIndex, &interaction);
	ThrowIfAUGraphError(result, "AUGraphGetInteractionInfo");
	return interaction;
}

UInt32 CXXAudioToolbox::ATAUGraph::CountNodeInteractions(AUNode inNode) const
{
	UInt32 numberOfInteractions = 0;
	auto result = AUGraphCountNodeInteractions(auGraph_, inNode, &numberOfInteractions);
	ThrowIfAUGraphError(result, "AUGraphCountNodeInteractions");
	return numberOfInteractions;
}

void CXXAudioToolbox::ATAUGraph::GetNodeInteractions(AUNode inNode, UInt32 *ioNumInteractions, AUNodeInteraction *outInteractions) const
{
	auto result = AUGraphGetNodeInteractions(auGraph_, inNode, ioNumInteractions, outInteractions);
	ThrowIfAUGraphError(result, "AUGraphGetNodeInteractions");
}

// MARK: -

bool CXXAudioToolbox::ATAUGraph::Update()
{
	Boolean flag = 0;
	auto result = AUGraphUpdate(auGraph_, &flag);
	ThrowIfAUGraphError(result, "AUGraphUpdate");
	return flag != 0;
}

// MARK: - State Management

void CXXAudioToolbox::ATAUGraph::Open()
{
	auto result = AUGraphOpen(auGraph_);
	ThrowIfAUGraphError(result, "AUGraphOpen");
}

void CXXAudioToolbox::ATAUGraph::Close()
{
	auto result = AUGraphClose(auGraph_);
	ThrowIfAUGraphError(result, "AUGraphClose");
}

void CXXAudioToolbox::ATAUGraph::Initialize()
{
	auto result = AUGraphInitialize(auGraph_);
	ThrowIfAUGraphError(result, "AUGraphInitialize");
}

void CXXAudioToolbox::ATAUGraph::Uninitialize()
{
	auto result = AUGraphUninitialize(auGraph_);
	ThrowIfAUGraphError(result, "AUGraphUninitialize");
}

void CXXAudioToolbox::ATAUGraph::Start()
{
	auto result = AUGraphStart(auGraph_);
	ThrowIfAUGraphError(result, "AUGraphStart");
}

void CXXAudioToolbox::ATAUGraph::Stop()
{
	auto result = AUGraphStop(auGraph_);
	ThrowIfAUGraphError(result, "AUGraphStop");
}

bool CXXAudioToolbox::ATAUGraph::IsOpen() const
{
	Boolean flag = 0;
	auto result = AUGraphIsOpen(auGraph_, &flag);
	ThrowIfAUGraphError(result, "AUGraphIsOpen");
	return flag != 0;
}

bool CXXAudioToolbox::ATAUGraph::IsInitialized() const
{
	Boolean flag = 0;
	auto result = AUGraphIsInitialized(auGraph_, &flag);
	ThrowIfAUGraphError(result, "AUGraphIsInitialized");
	return flag != 0;
}

bool CXXAudioToolbox::ATAUGraph::IsRunning() const
{
	Boolean flag = 0;
	auto result = AUGraphIsRunning(auGraph_, &flag);
	ThrowIfAUGraphError(result, "AUGraphIsRunning");
	return flag != 0;
}

// MARK: - Utilities

Float32 CXXAudioToolbox::ATAUGraph::GetCPULoad() const
{
	Float32 value = 0;
	auto result = AUGraphGetCPULoad(auGraph_, &value);
	ThrowIfAUGraphError(result, "AUGraphGetCPULoad");
	return value;
}

Float32 CXXAudioToolbox::ATAUGraph::GetMaxCPULoad() const
{
	Float32 value = 0;
	auto result = AUGraphGetMaxCPULoad(auGraph_, &value);
	ThrowIfAUGraphError(result, "AUGraphGetMaxCPULoad");
	return value;
}

void CXXAudioToolbox::ATAUGraph::AddRenderNotify(AURenderCallback inCallback, void *inRefCon)
{
	auto result = AUGraphAddRenderNotify(auGraph_, inCallback, inRefCon);
	ThrowIfAUGraphError(result, "AUGraphAddRenderNotify");
}

void CXXAudioToolbox::ATAUGraph::RemoveRenderNotify(AURenderCallback inCallback, void *inRefCon)
{
	auto result = AUGraphRemoveRenderNotify(auGraph_, inCallback, inRefCon);
	ThrowIfAUGraphError(result, "AUGraphRemoveRenderNotify");
}

// MARK: - Helpers

std::vector<AUNode> CXXAudioToolbox::ATAUGraph::Nodes() const
{
	auto nodeCount = GetNodeCount();
	auto nodes = std::vector<AUNode>(nodeCount);
	for(UInt32 i = 0; i < nodeCount; ++i) {
		auto node = GetIndNode(i);
		nodes.push_back(node);
	}
	return nodes;
}

std::vector<AUNodeInteraction> CXXAudioToolbox::ATAUGraph::NodeInteractions(AUNode inNode) const
{
	auto interactionCount = CountNodeInteractions(inNode);
	auto interactions = std::vector<AUNodeInteraction>(interactionCount);
	GetNodeInteractions(inNode, &interactionCount, &interactions[0]);
	return interactions;
}

std::map<AUNode, std::vector<AUNodeInteraction>> CXXAudioToolbox::ATAUGraph::NodesAndInteractions() const
{
	auto nodes = Nodes();
	auto nodesAndInteractions = std::map<AUNode, std::vector<AUNodeInteraction>>();
	for(auto node : nodes)
		nodesAndInteractions[node] = NodeInteractions(node);
	return nodesAndInteractions;
}

Float64 CXXAudioToolbox::ATAUGraph::Latency() const
{
	Float64 latency = 0;
	auto nodeCount = GetNodeCount();
	for(UInt32 i = 0; i < nodeCount; ++i) {
		auto node = GetIndNode(i);
		AudioUnit au = nullptr;
		NodeInfo(node, nullptr, &au);

		Float64 auLatency = 0;
		UInt32 dataSize = sizeof(auLatency);
		auto result = AudioUnitGetProperty(au, kAudioUnitProperty_Latency, kAudioUnitScope_Global, 0, &auLatency, &dataSize);
		ThrowIfAudioUnitError(result, "AudioUnitGetProperty (kAudioUnitProperty_Latency, kAudioUnitScope_Global)");

		latency += auLatency;
	}

	return latency;
}

Float64 CXXAudioToolbox::ATAUGraph::TailTime() const
{
	Float64 tailTime = 0;
	auto nodeCount = GetNodeCount();
	for(UInt32 i = 0; i < nodeCount; ++i) {
		auto node = GetIndNode(i);
		AudioUnit au = nullptr;
		NodeInfo(node, nullptr, &au);

		Float64 auTailTime = 0;
		UInt32 dataSize = sizeof(auTailTime);
		auto result = AudioUnitGetProperty(au, kAudioUnitProperty_TailTime, kAudioUnitScope_Global, 0, &auTailTime, &dataSize);
		ThrowIfAudioUnitError(result, "AudioUnitGetProperty (kAudioUnitProperty_TailTime, kAudioUnitScope_Global)");

		tailTime += auTailTime;
	}

	return tailTime;
}
