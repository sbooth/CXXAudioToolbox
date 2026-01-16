//
// SPDX-FileCopyrightText: 2021 Stephen F. Booth <contact@sbooth.dev>
// SPDX-License-Identifier: MIT
//
// Part of https://github.com/sbooth/CXXAudioToolbox
//

#import "CAAUGraph.hpp"

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

std::expected<void, OSStatus> CXXAudioToolbox::CAAUGraph::New() noexcept
{
	Dispose();
	const auto result = NewAUGraph(&graph_);
	if(result != noErr)
		return std::unexpected(result);
}

std::expected<void, OSStatus> CXXAudioToolbox::CAAUGraph::Dispose() noexcept
{
	if(graph_) {
		const auto result = DisposeAUGraph(graph_);
		graph_ = nullptr;
		if(result != noErr)
			return std::unexpected(result);
	}
}

// MARK: - Node State

std::expected<AUNode, OSStatus> CXXAudioToolbox::CAAUGraph::AddNode(const AudioComponentDescription *inDescription) noexcept
{
	AUNode node{-1};
	const auto result = AUGraphAddNode(graph_, inDescription, &node);
	if(result != noErr)
		return std::unexpected(result);
	return node;
}

std::expected<void, OSStatus> CXXAudioToolbox::CAAUGraph::RemoveNode(AUNode inNode) noexcept
{
	const auto result = AUGraphRemoveNode(graph_, inNode);
	if(result != noErr)
		return std::unexpected(result);
	return {};
}

std::expected<UInt32, OSStatus> CXXAudioToolbox::CAAUGraph::GetNodeCount() const noexcept
{
	UInt32 numberOfNodes = 0;
	const auto result = AUGraphGetNodeCount(graph_, &numberOfNodes);
	if(result != noErr)
		return std::unexpected(result);
	return numberOfNodes;
}

std::expected<AUNode, OSStatus> CXXAudioToolbox::CAAUGraph::GetIndNode(UInt32 inIndex) const noexcept
{
	AUNode node = -1;
	const auto result = AUGraphGetIndNode(graph_, inIndex, &node);
	if(result != noErr)
		return std::unexpected(result);
	return node;
}

std::expected<void, OSStatus> CXXAudioToolbox::CAAUGraph::NodeInfo(AUNode inNode, AudioComponentDescription *outDescription, AudioUnit *outAudioUnit) const noexcept
{
	const auto result = AUGraphNodeInfo(graph_, inNode, outDescription, outAudioUnit);
	if(result != noErr)
		return std::unexpected(result);
	return {};
}


#if !TARGET_OS_IPHONE
// MARK: - Sub Graphs

std::expected<AUNode, OSStatus> CXXAudioToolbox::CAAUGraph::NewNodeSubGraph() noexcept
{
	AUNode node = -1;
	const auto result = AUGraphNewNodeSubGraph(graph_, &node);
	if(result != noErr)
		return std::unexpected(result);
	return node;
}

std::expected<AUGraph, OSStatus> CXXAudioToolbox::CAAUGraph::GetNodeInfoSubGraph(AUNode inNode) const noexcept
{
	AUGraph subGraph = nullptr;
	const auto result = AUGraphGetNodeInfoSubGraph(graph_, inNode, &subGraph);
	if(result != noErr)
		return std::unexpected(result);
	return subGraph;
}

std::expected<bool, OSStatus> CXXAudioToolbox::CAAUGraph::IsNodeSubGraph(AUNode inNode) const noexcept
{
	Boolean flag = 0;
	const auto result = AUGraphIsNodeSubGraph(graph_, inNode, &flag);
	if(result != noErr)
		return std::unexpected(result);
	return flag != 0;
}
#endif /* !TARGET_OS_IPHONE */

// MARK: - Node Interactions

std::expected<void, OSStatus> CXXAudioToolbox::CAAUGraph::ConnectNodeInput(AUNode inSourceNode, UInt32 inSourceOutputNumber, AUNode inDestNode, UInt32 inDestInputNumber) noexcept
{
	const auto result = AUGraphConnectNodeInput(graph_, inSourceNode, inSourceOutputNumber, inDestNode, inDestInputNumber);
	if(result != noErr)
		return std::unexpected(result);
	return {};
}

std::expected<void, OSStatus> CXXAudioToolbox::CAAUGraph::SetNodeInputCallback(AUNode inDestNode, UInt32 inDestInputNumber, const AURenderCallbackStruct *inInputCallback) noexcept
{
	const auto result = AUGraphSetNodeInputCallback(graph_, inDestNode, inDestInputNumber, inInputCallback);
	if(result != noErr)
		return std::unexpected(result);
	return {};
}

std::expected<void, OSStatus> CXXAudioToolbox::CAAUGraph::DisconnectNodeInput(AUNode inDestNode, UInt32 inDestInputNumber) noexcept
{
	const auto result = AUGraphDisconnectNodeInput(graph_, inDestNode, inDestInputNumber);
	if(result != noErr)
		return std::unexpected(result);
	return {};
}

std::expected<void, OSStatus> CXXAudioToolbox::CAAUGraph::ClearConnections() noexcept
{
	const auto result = AUGraphClearConnections(graph_);
	if(result != noErr)
		return std::unexpected(result);
	return {};
}

std::expected<UInt32, OSStatus> CXXAudioToolbox::CAAUGraph::GetNumberOfInteractions() const noexcept
{
	UInt32 numberOfInteractions = 0;
	const auto result = AUGraphGetNumberOfInteractions(graph_, &numberOfInteractions);
	if(result != noErr)
		return std::unexpected(result);
	return numberOfInteractions;
}

std::expected<AUNodeInteraction, OSStatus> CXXAudioToolbox::CAAUGraph::GetInteractionInfo(UInt32 inInteractionIndex) const noexcept
{
	AUNodeInteraction interaction{};
	const auto result = AUGraphGetInteractionInfo(graph_, inInteractionIndex, &interaction);
	if(result != noErr)
		return std::unexpected(result);
	return interaction;
}

std::expected<UInt32, OSStatus> CXXAudioToolbox::CAAUGraph::CountNodeInteractions(AUNode inNode) const noexcept
{
	UInt32 numberOfInteractions = 0;
	const auto result = AUGraphCountNodeInteractions(graph_, inNode, &numberOfInteractions);
	if(result != noErr)
		return std::unexpected(result);
	return numberOfInteractions;
}

std::expected<void, OSStatus> CXXAudioToolbox::CAAUGraph::GetNodeInteractions(AUNode inNode, UInt32 *ioNumInteractions, AUNodeInteraction *outInteractions) const noexcept
{
	const auto result = AUGraphGetNodeInteractions(graph_, inNode, ioNumInteractions, outInteractions);
	if(result != noErr)
		return std::unexpected(result);
	return {};
}

// MARK: -

std::expected<bool, OSStatus> CXXAudioToolbox::CAAUGraph::Update() noexcept
{
	Boolean flag = 0;
	const auto result = AUGraphUpdate(graph_, &flag);
	if(result != noErr)
		return std::unexpected(result);
	return flag != 0;
}

// MARK: - State Management

std::expected<void, OSStatus> CXXAudioToolbox::CAAUGraph::Open() noexcept
{
	const auto result = AUGraphOpen(graph_);
	if(result != noErr)
		return std::unexpected(result);
	return {};
}

std::expected<void, OSStatus> CXXAudioToolbox::CAAUGraph::Close() noexcept
{
	const auto result = AUGraphClose(graph_);
	if(result != noErr)
		return std::unexpected(result);
	return {};
}

std::expected<void, OSStatus> CXXAudioToolbox::CAAUGraph::Initialize() noexcept
{
	const auto result = AUGraphInitialize(graph_);
	if(result != noErr)
		return std::unexpected(result);
	return {};
}

std::expected<void, OSStatus> CXXAudioToolbox::CAAUGraph::Uninitialize() noexcept
{
	const auto result = AUGraphUninitialize(graph_);
	if(result != noErr)
		return std::unexpected(result);
	return {};
}

std::expected<void, OSStatus> CXXAudioToolbox::CAAUGraph::Start() noexcept
{
	const auto result = AUGraphStart(graph_);
	if(result != noErr)
		return std::unexpected(result);
	return {};
}

std::expected<void, OSStatus> CXXAudioToolbox::CAAUGraph::Stop() noexcept
{
	const auto result = AUGraphStop(graph_);
	if(result != noErr)
		return std::unexpected(result);
	return {};
}

std::expected<bool, OSStatus> CXXAudioToolbox::CAAUGraph::IsOpen() const noexcept
{
	Boolean flag = 0;
	const auto result = AUGraphIsOpen(graph_, &flag);
	if(result != noErr)
		return std::unexpected(result);
	return flag != 0;
}

std::expected<bool, OSStatus> CXXAudioToolbox::CAAUGraph::IsInitialized() const noexcept
{
	Boolean flag = 0;
	const auto result = AUGraphIsInitialized(graph_, &flag);
	if(result != noErr)
		return std::unexpected(result);
	return flag != 0;
}

std::expected<bool, OSStatus> CXXAudioToolbox::CAAUGraph::IsRunning() const noexcept
{
	Boolean flag = 0;
	const auto result = AUGraphIsRunning(graph_, &flag);
	if(result != noErr)
		return std::unexpected(result);
	return flag != 0;
}

// MARK: - Utilities

std::expected<Float32, OSStatus> CXXAudioToolbox::CAAUGraph::GetCPULoad() const noexcept
{
	Float32 value = 0;
	const auto result = AUGraphGetCPULoad(graph_, &value);
	if(result != noErr)
		return std::unexpected(result);
	return value;
}

std::expected<Float32, OSStatus> CXXAudioToolbox::CAAUGraph::GetMaxCPULoad() const noexcept
{
	Float32 value = 0;
	const auto result = AUGraphGetMaxCPULoad(graph_, &value);
	if(result != noErr)
		return std::unexpected(result);
	return value;
}

std::expected<void, OSStatus> CXXAudioToolbox::CAAUGraph::AddRenderNotify(AURenderCallback inCallback, void *inRefCon) noexcept
{
	const auto result = AUGraphAddRenderNotify(graph_, inCallback, inRefCon);
	if(result != noErr)
		return std::unexpected(result);
	return {};
}

std::expected<void, OSStatus> CXXAudioToolbox::CAAUGraph::RemoveRenderNotify(AURenderCallback inCallback, void *inRefCon) noexcept
{
	const auto result = AUGraphRemoveRenderNotify(graph_, inCallback, inRefCon);
	if(result != noErr)
		return std::unexpected(result);
	return {};
}

// MARK: - Helpers

std::expected<std::vector<AUNode>, OSStatus> CXXAudioToolbox::CAAUGraph::Nodes() const noexcept
{
	UInt32 nodeCount;
	auto result = AUGraphGetNodeCount(graph_, &nodeCount);
	if(result != noErr)
		return std::unexpected(result);

	try {
		auto nodes = std::vector<AUNode>(nodeCount);
		for(UInt32 i = 0; i < nodeCount; ++i) {
			AUNode node;
			auto result = AUGraphGetIndNode(graph_, i, &node);
			if(result != noErr)
				return std::unexpected(result);
			nodes.push_back(node);
		}
		return nodes;
	} catch(const std::exception& e) {
		return std::unexpected(kAudio_MemFullError);
	}
}

std::expected<std::vector<AUNodeInteraction>, OSStatus> CXXAudioToolbox::CAAUGraph::NodeInteractions(AUNode inNode) const noexcept
{
	try {
		UInt32 interactionCount;
		auto result = AUGraphCountNodeInteractions(graph_, inNode, &interactionCount);
		if(result != noErr)
			return std::unexpected(result);

		auto interactions = std::vector<AUNodeInteraction>(interactionCount);
		result = AUGraphGetNodeInteractions(graph_, inNode, &interactionCount, interactions.data());
		if(result != noErr)
			return std::unexpected(result);
		interactions.resize(interactionCount);
		return interactions;
	} catch(const std::exception& e) {
		return std::unexpected(kAudio_MemFullError);
	}
}

std::expected<std::map<AUNode, std::vector<AUNodeInteraction>>, OSStatus> CXXAudioToolbox::CAAUGraph::NodesAndInteractions() const noexcept
{
	UInt32 nodeCount;
	auto result = AUGraphGetNodeCount(graph_, &nodeCount);
	if(result != noErr)
		return std::unexpected(result);

	try {
		auto nodesAndInteractions = std::map<AUNode, std::vector<AUNodeInteraction>>();
		for(UInt32 i = 0; i < nodeCount; ++i) {
			AUNode node;
			auto result = AUGraphGetIndNode(graph_, i, &node);
			if(result != noErr)
				return std::unexpected(result);

			UInt32 interactionCount;
			result = AUGraphCountNodeInteractions(graph_, node, &interactionCount);
			if(result != noErr)
				return std::unexpected(result);

			auto interactions = std::vector<AUNodeInteraction>(interactionCount);
			result = AUGraphGetNodeInteractions(graph_, node, &interactionCount, interactions.data());
			if(result != noErr)
				return std::unexpected(result);

			interactions.resize(interactionCount);
			nodesAndInteractions[node] = interactions;
		}

		return nodesAndInteractions;
	} catch(const std::exception& e) {
		return std::unexpected(kAudio_MemFullError);
	}
}

std::expected<Float64, OSStatus> CXXAudioToolbox::CAAUGraph::Latency() const noexcept
{
	UInt32 nodeCount;
	auto result = AUGraphGetNodeCount(graph_, &nodeCount);
	if(result != noErr)
		return std::unexpected(result);

	try {
		Float64 latency = 0;
		for(UInt32 i = 0; i < nodeCount; ++i) {
			AUNode node;
			result = AUGraphGetIndNode(graph_, i, &node);
			if(result != noErr)
				return std::unexpected(result);

			AudioUnit au;
			result = AUGraphNodeInfo(graph_, node, nullptr, &au);
			if(result != noErr)
				return std::unexpected(result);

			Float64 auLatency = 0;
			UInt32 dataSize = sizeof auLatency;
			result = AudioUnitGetProperty(au, kAudioUnitProperty_Latency, kAudioUnitScope_Global, 0, &auLatency, &dataSize);
			if(result != noErr)
				return std::unexpected(result);

			latency += auLatency;
		}
		return latency;
	} catch(const std::exception& e) {
		return std::unexpected(kAudio_MemFullError);
	}
}

std::expected<Float64, OSStatus> CXXAudioToolbox::CAAUGraph::TailTime() const noexcept
{
	UInt32 nodeCount;
	auto result = AUGraphGetNodeCount(graph_, &nodeCount);
	if(result != noErr)
		return std::unexpected(result);

	try {
		Float64 tailTime = 0;
		for(UInt32 i = 0; i < nodeCount; ++i) {
			AUNode node;
			result = AUGraphGetIndNode(graph_, i, &node);
			if(result != noErr)
				return std::unexpected(result);

			AudioUnit au;
			result = AUGraphNodeInfo(graph_, node, nullptr, &au);
			if(result != noErr)
				return std::unexpected(result);

			Float64 auTailTime = 0;
			UInt32 dataSize = sizeof auTailTime;
			result = AudioUnitGetProperty(au, kAudioUnitProperty_TailTime, kAudioUnitScope_Global, 0, &auTailTime, &dataSize);
			if(result != noErr)
				return std::unexpected(result);

			tailTime += auTailTime;
		}
		return tailTime;
	} catch(const std::exception& e) {
		return std::unexpected(kAudio_MemFullError);
	}
}
