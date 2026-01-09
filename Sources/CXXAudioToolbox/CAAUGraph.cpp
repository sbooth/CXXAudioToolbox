//
// SPDX-FileCopyrightText: 2021 Stephen F. Booth <contact@sbooth.dev>
// SPDX-License-Identifier: MIT
//
// Part of https://github.com/sbooth/CXXAudioToolbox
//

#import "CAAUGraph.hpp"
#import "AudioToolboxErrors.hpp"
#import "StringFormatting.hpp"

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
	CXXAudioToolbox_ThrowIfAUGraphError(result, concat({
		"NewAUGraph"
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
}

void CXXAudioToolbox::CAAUGraph::Dispose()
{
	if(graph_) {
		const auto result = DisposeAUGraph(graph_);
		graph_ = nullptr;
		CXXAudioToolbox_ThrowIfAUGraphError(result, concat({
			"DisposeAUGraph"
			" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
		}));
	}
}

// MARK: - Node State

AUNode CXXAudioToolbox::CAAUGraph::AddNode(const AudioComponentDescription *inDescription)
{
	AUNode node{-1};
	const auto result = AUGraphAddNode(graph_, inDescription, &node);
	CXXAudioToolbox_ThrowIfAUGraphError(result, concat({
		"AUGraphAddNode("
		"0x", to_hex_string(reinterpret_cast<uintptr_t>(inDescription)),
		")",
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
	return node;
}

void CXXAudioToolbox::CAAUGraph::RemoveNode(AUNode inNode)
{
	const auto result = AUGraphRemoveNode(graph_, inNode);
	CXXAudioToolbox_ThrowIfAUGraphError(result, concat({
		"AUGraphRemoveNode(",
		to_string(inNode),
		")",
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
}

UInt32 CXXAudioToolbox::CAAUGraph::GetNodeCount() const
{
	UInt32 numberOfNodes = 0;
	const auto result = AUGraphGetNodeCount(graph_, &numberOfNodes);
	CXXAudioToolbox_ThrowIfAUGraphError(result, concat({
		"AUGraphGetNodeCount"
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
	return numberOfNodes;
}

AUNode CXXAudioToolbox::CAAUGraph::GetIndNode(UInt32 inIndex) const
{
	AUNode node = -1;
	const auto result = AUGraphGetIndNode(graph_, inIndex, &node);
	CXXAudioToolbox_ThrowIfAUGraphError(result, concat({
		"AUGraphGetIndNode(",
		to_string(inIndex),
		")",
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
	return node;
}

void CXXAudioToolbox::CAAUGraph::NodeInfo(AUNode inNode, AudioComponentDescription *outDescription, AudioUnit *outAudioUnit) const
{
	const auto result = AUGraphNodeInfo(graph_, inNode, outDescription, outAudioUnit);
	CXXAudioToolbox_ThrowIfAUGraphError(result, concat({
		"AUGraphNodeInfo(",
		to_string(inNode),
		", 0x", to_hex_string(reinterpret_cast<uintptr_t>(outDescription)),
		", 0x", to_hex_string(reinterpret_cast<uintptr_t>(outAudioUnit)),
		")",
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
}


#if !TARGET_OS_IPHONE
// MARK: - Sub Graphs

AUNode CXXAudioToolbox::CAAUGraph::NewNodeSubGraph()
{
	AUNode node = -1;
	const auto result = AUGraphNewNodeSubGraph(graph_, &node);
	CXXAudioToolbox_ThrowIfAUGraphError(result, concat({
		"AUGraphNewNodeSubGraph"
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
	return node;
}

AUGraph CXXAudioToolbox::CAAUGraph::GetNodeInfoSubGraph(AUNode inNode) const
{
	AUGraph subGraph = nullptr;
	const auto result = AUGraphGetNodeInfoSubGraph(graph_, inNode, &subGraph);
	CXXAudioToolbox_ThrowIfAUGraphError(result, concat({
		"AUGraphGetNodeInfoSubGraph(",
		to_string(inNode),
		")",
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
	return subGraph;
}

bool CXXAudioToolbox::CAAUGraph::IsNodeSubGraph(AUNode inNode) const
{
	Boolean flag = 0;
	const auto result = AUGraphIsNodeSubGraph(graph_, inNode, &flag);
	CXXAudioToolbox_ThrowIfAUGraphError(result, concat({
		"AUGraphIsNodeSubGraph(",
		to_string(inNode),
		")",
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
	return flag != 0;
}
#endif /* !TARGET_OS_IPHONE */

// MARK: - Node Interactions

void CXXAudioToolbox::CAAUGraph::ConnectNodeInput(AUNode inSourceNode, UInt32 inSourceOutputNumber, AUNode inDestNode, UInt32 inDestInputNumber)
{
	const auto result = AUGraphConnectNodeInput(graph_, inSourceNode, inSourceOutputNumber, inDestNode, inDestInputNumber);
	CXXAudioToolbox_ThrowIfAUGraphError(result, concat({
		"AUGraphConnectNodeInput(",
		to_string(inSourceNode),
		to_string(inSourceOutputNumber),
		to_string(inDestNode),
		to_string(inDestInputNumber),
		")",
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
}

void CXXAudioToolbox::CAAUGraph::SetNodeInputCallback(AUNode inDestNode, UInt32 inDestInputNumber, const AURenderCallbackStruct *inInputCallback)
{
	const auto result = AUGraphSetNodeInputCallback(graph_, inDestNode, inDestInputNumber, inInputCallback);
	CXXAudioToolbox_ThrowIfAUGraphError(result, concat({
		"AUGraphSetNodeInputCallback(",
		to_string(inDestNode),
		to_string(inDestInputNumber),
		", 0x", to_hex_string(reinterpret_cast<uintptr_t>(inInputCallback)),
		")",
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
}

void CXXAudioToolbox::CAAUGraph::DisconnectNodeInput(AUNode inDestNode, UInt32 inDestInputNumber)
{
	const auto result = AUGraphDisconnectNodeInput(graph_, inDestNode, inDestInputNumber);
	CXXAudioToolbox_ThrowIfAUGraphError(result, concat({
		"AUGraphDisconnectNodeInput(",
		to_string(inDestNode),
		to_string(inDestInputNumber),
		")",
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
}

void CXXAudioToolbox::CAAUGraph::ClearConnections()
{
	const auto result = AUGraphClearConnections(graph_);
	CXXAudioToolbox_ThrowIfAUGraphError(result, concat({
		"AUGraphClearConnections"
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
}

UInt32 CXXAudioToolbox::CAAUGraph::GetNumberOfInteractions() const
{
	UInt32 numberOfInteractions = 0;
	const auto result = AUGraphGetNumberOfInteractions(graph_, &numberOfInteractions);
	CXXAudioToolbox_ThrowIfAUGraphError(result, concat({
		"AUGraphGetNumberOfInteractions"
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
	return numberOfInteractions;
}

AUNodeInteraction CXXAudioToolbox::CAAUGraph::GetInteractionInfo(UInt32 inInteractionIndex) const
{
	AUNodeInteraction interaction{};
	const auto result = AUGraphGetInteractionInfo(graph_, inInteractionIndex, &interaction);
	CXXAudioToolbox_ThrowIfAUGraphError(result, concat({
		"AUGraphGetInteractionInfo(",
		to_string(inInteractionIndex),
		")",
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
	return interaction;
}

UInt32 CXXAudioToolbox::CAAUGraph::CountNodeInteractions(AUNode inNode) const
{
	UInt32 numberOfInteractions = 0;
	const auto result = AUGraphCountNodeInteractions(graph_, inNode, &numberOfInteractions);
	CXXAudioToolbox_ThrowIfAUGraphError(result, concat({
		"AUGraphCountNodeInteractions(",
		to_string(inNode),
		")",
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
	return numberOfInteractions;
}

void CXXAudioToolbox::CAAUGraph::GetNodeInteractions(AUNode inNode, UInt32 *ioNumInteractions, AUNodeInteraction *outInteractions) const
{
	const auto result = AUGraphGetNodeInteractions(graph_, inNode, ioNumInteractions, outInteractions);
	CXXAudioToolbox_ThrowIfAUGraphError(result, concat({
		"AUGraphGetNodeInteractions(",
		to_string(inNode),
		", 0x", to_hex_string(reinterpret_cast<uintptr_t>(ioNumInteractions)),
		", 0x", to_hex_string(reinterpret_cast<uintptr_t>(outInteractions)),
		")",
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
}

// MARK: -

bool CXXAudioToolbox::CAAUGraph::Update()
{
	Boolean flag = 0;
	const auto result = AUGraphUpdate(graph_, &flag);
	CXXAudioToolbox_ThrowIfAUGraphError(result, concat({
		"AUGraphUpdate"
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
	return flag != 0;
}

// MARK: - State Management

void CXXAudioToolbox::CAAUGraph::Open()
{
	const auto result = AUGraphOpen(graph_);
	CXXAudioToolbox_ThrowIfAUGraphError(result, concat({
		"AUGraphOpen"
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
}

void CXXAudioToolbox::CAAUGraph::Close()
{
	const auto result = AUGraphClose(graph_);
	CXXAudioToolbox_ThrowIfAUGraphError(result, concat({
		"AUGraphClose"
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
}

void CXXAudioToolbox::CAAUGraph::Initialize()
{
	const auto result = AUGraphInitialize(graph_);
	CXXAudioToolbox_ThrowIfAUGraphError(result, concat({
		"AUGraphInitialize"
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
}

void CXXAudioToolbox::CAAUGraph::Uninitialize()
{
	const auto result = AUGraphUninitialize(graph_);
	CXXAudioToolbox_ThrowIfAUGraphError(result, concat({
		"AUGraphUninitialize"
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
}

void CXXAudioToolbox::CAAUGraph::Start()
{
	const auto result = AUGraphStart(graph_);
	CXXAudioToolbox_ThrowIfAUGraphError(result, concat({
		"AUGraphStart"
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
}

void CXXAudioToolbox::CAAUGraph::Stop()
{
	const auto result = AUGraphStop(graph_);
	CXXAudioToolbox_ThrowIfAUGraphError(result, concat({
		"AUGraphStop"
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
}

bool CXXAudioToolbox::CAAUGraph::IsOpen() const
{
	Boolean flag = 0;
	const auto result = AUGraphIsOpen(graph_, &flag);
	CXXAudioToolbox_ThrowIfAUGraphError(result, concat({
		"AUGraphIsOpen"
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
	return flag != 0;
}

bool CXXAudioToolbox::CAAUGraph::IsInitialized() const
{
	Boolean flag = 0;
	const auto result = AUGraphIsInitialized(graph_, &flag);
	CXXAudioToolbox_ThrowIfAUGraphError(result, concat({
		"AUGraphIsInitialized"
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
	return flag != 0;
}

bool CXXAudioToolbox::CAAUGraph::IsRunning() const
{
	Boolean flag = 0;
	const auto result = AUGraphIsRunning(graph_, &flag);
	CXXAudioToolbox_ThrowIfAUGraphError(result, concat({
		"AUGraphIsRunning"
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
	return flag != 0;
}

// MARK: - Utilities

Float32 CXXAudioToolbox::CAAUGraph::GetCPULoad() const
{
	Float32 value = 0;
	const auto result = AUGraphGetCPULoad(graph_, &value);
	CXXAudioToolbox_ThrowIfAUGraphError(result, concat({
		"AUGraphGetCPULoad"
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
	return value;
}

Float32 CXXAudioToolbox::CAAUGraph::GetMaxCPULoad() const
{
	Float32 value = 0;
	const auto result = AUGraphGetMaxCPULoad(graph_, &value);
	CXXAudioToolbox_ThrowIfAUGraphError(result, concat({
		"AUGraphGetMaxCPULoad"
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
	return value;
}

void CXXAudioToolbox::CAAUGraph::AddRenderNotify(AURenderCallback inCallback, void *inRefCon)
{
	const auto result = AUGraphAddRenderNotify(graph_, inCallback, inRefCon);
	CXXAudioToolbox_ThrowIfAUGraphError(result, concat({
		"AUGraphAddRenderNotify("
		"0x", to_hex_string(reinterpret_cast<uintptr_t>(inCallback)),
		", 0x", to_hex_string(reinterpret_cast<uintptr_t>(inRefCon)),
		")",
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
}

void CXXAudioToolbox::CAAUGraph::RemoveRenderNotify(AURenderCallback inCallback, void *inRefCon)
{
	const auto result = AUGraphRemoveRenderNotify(graph_, inCallback, inRefCon);
	CXXAudioToolbox_ThrowIfAUGraphError(result, concat({
		"AUGraphRemoveRenderNotify("
		"0x", to_hex_string(reinterpret_cast<uintptr_t>(inCallback)),
		", 0x", to_hex_string(reinterpret_cast<uintptr_t>(inRefCon)),
		")",
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
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
		CXXAudioToolbox_ThrowIfAudioUnitError(result, concat({
			"AudioUnitGetProperty(kAudioUnitProperty_Latency, kAudioUnitScope_Global)",
			" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
		}));

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
		CXXAudioToolbox_ThrowIfAudioUnitError(result, concat({
			"AudioUnitGetProperty(kAudioUnitProperty_TailTime, kAudioUnitScope_Global)",
			" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
		}));

		tailTime += auTailTime;
	}

	return tailTime;
}
