//
// SPDX-FileCopyrightText: 2021 Stephen F. Booth <contact@sbooth.dev>
// SPDX-License-Identifier: MIT
//
// Part of https://github.com/sbooth/CXXAudioToolbox
//

#pragma once

#import <expected>
#import <map>
#import <utility>
#import <vector>

#import <AudioToolbox/AUGraph.h>

#import <CXXCoreAudio/CAStreamDescription.hpp>

CF_ASSUME_NONNULL_BEGIN

namespace CXXAudioToolbox {

/// An AUGraph wrapper.
class CAAUGraph final {
public:
	// MARK: Creation and Destruction

	/// Creates an Audio Unit graph.
	CAAUGraph() noexcept = default;

	// This class is non-copyable
	CAAUGraph(const CAAUGraph&) = delete;

	// This class is non-assignable
	CAAUGraph& operator=(const CAAUGraph&) = delete;

	/// Move constructor
	CAAUGraph(CAAUGraph&& other) noexcept;

	/// Move assignment operator.
	CAAUGraph& operator=(CAAUGraph&& other) noexcept;

	/// Destroys the Audio Unit graph and releases all associated resources.
	~CAAUGraph() noexcept;


	/// Returns true if the managed AUGraph object is not null.
	[[nodiscard]] explicit operator bool() const noexcept;

	/// Returns the managed AUGraph object.
	[[nodiscard]] operator AUGraph const _Nullable () const noexcept;


	/// Creates a new Audio Unit graph.
	std::expected<void, OSStatus> New() noexcept;

	/// Disposes the Audio Unit graph.
	std::expected<void, OSStatus> Dispose() noexcept;

	// MARK: - Node State

	/// Adds a node to the Audio Unit graph.
	std::expected<AUNode, OSStatus> AddNode(const AudioComponentDescription *inDescription) noexcept;

	/// Removes a node from the Audio Unit graph.
	std::expected<void, OSStatus> RemoveNode(AUNode inNode) noexcept;

	/// Returns the number of nodes in the Audio Unit graph.
	[[nodiscard]] std::expected<UInt32, OSStatus> GetNodeCount() const noexcept;

	/// Returns the node at a given index
	[[nodiscard]] std::expected<AUNode, OSStatus> GetIndNode(UInt32 inIndex) const noexcept;

	/// Returns information about a particular AUNode.
	std::expected<void, OSStatus> NodeInfo(AUNode inNode, AudioComponentDescription * _Nullable outDescription, AudioUnit _Nullable * _Nullable outAudioUnit) const noexcept;

#if !TARGET_OS_IPHONE
	// MARK: - Sub Graphs

	/// Creates a node that will represent a sub graph.
	std::expected<AUNode, OSStatus> NewNodeSubGraph() noexcept;

	/// Returns the sub graph represented by a particular AUNode.
	[[nodiscard]] std::expected<AUGraph, OSStatus> GetNodeInfoSubGraph(AUNode inNode) const noexcept;

	/// Returns true if the node represents a sub graph.
	[[nodiscard]] std::expected<bool, OSStatus> IsNodeSubGraph(AUNode inNode) const noexcept;
#endif /* !TARGET_OS_IPHONE */

	// MARK: - Node Interactions

	/// Connects a node's output to a node's input.
	std::expected<void, OSStatus> ConnectNodeInput(AUNode inSourceNode, UInt32 inSourceOutputNumber, AUNode inDestNode, UInt32 inDestInputNumber) noexcept;

	/// Sets a callback for the specified node's specified input.
	std::expected<void, OSStatus> SetNodeInputCallback(AUNode inDestNode, UInt32 inDestInputNumber, const AURenderCallbackStruct *inInputCallback) noexcept;

	/// Disconnects a node's input.
	std::expected<void, OSStatus> DisconnectNodeInput(AUNode inDestNode, UInt32 inDestInputNumber) noexcept;

	/// Clears all of the interactions in a graph.
	std::expected<void, OSStatus> ClearConnections() noexcept;

	/// Returns the number of interactions in the Audio Unit graph.
	[[nodiscard]] std::expected<UInt32, OSStatus> GetNumberOfInteractions() const noexcept;

	/// Returns information about a particular interaction in a graph.
	[[nodiscard]] std::expected<AUNodeInteraction, OSStatus> GetInteractionInfo(UInt32 inInteractionIndex) const noexcept;

	/// Returns the number of interactions of a graph's node.
	[[nodiscard]] std::expected<UInt32, OSStatus> CountNodeInteractions(AUNode inNode) const noexcept;

	/// Retrieves information about the interactions in a graph for a given node.
	std::expected<void, OSStatus> GetNodeInteractions(AUNode inNode, UInt32 *ioNumInteractions, AUNodeInteraction *outInteractions) const noexcept;

	// MARK: -

	/// Updates the state of a running AUGraph.
	std::expected<bool, OSStatus> Update() noexcept;

	// MARK: - State Management

	/// Opens a graph.
	std::expected<void, OSStatus> Open() noexcept;

	/// Closes a graph.
	std::expected<void, OSStatus> Close() noexcept;

	/// Initializes a graph.
	std::expected<void, OSStatus> Initialize() noexcept;

	/// Uninitialize a graph.
	std::expected<void, OSStatus> Uninitialize() noexcept;

	/// Starts a graph.
	std::expected<void, OSStatus> Start() noexcept;

	/// Stops a graph.
	std::expected<void, OSStatus> Stop() noexcept;

	/// Returns true if the Audio Unit graph is open.
	[[nodiscard]] std::expected<bool, OSStatus> IsOpen() const noexcept;

	/// Returns true if the Audio Unit graph is initialized.
	[[nodiscard]] std::expected<bool, OSStatus> IsInitialized() const noexcept;

	/// Returns true if the Audio Unit graph is running.
	[[nodiscard]] std::expected<bool, OSStatus> IsRunning() const noexcept;

	// MARK: - Utilities

	/// Returns a short-term running average of the current CPU load of the graph.
	[[nodiscard]] std::expected<Float32, OSStatus> GetCPULoad() const noexcept;

	/// Returns the max CPU load of the graph since this call was last made or the graph was last started.
	[[nodiscard]] std::expected<Float32, OSStatus> GetMaxCPULoad() const noexcept;

	/// Adds a notification callback.
	std::expected<void, OSStatus> AddRenderNotify(AURenderCallback inCallback, void * _Nullable inRefCon) noexcept;

	/// Removes a notification callback.
	std::expected<void, OSStatus> RemoveRenderNotify(AURenderCallback inCallback, void * _Nullable inRefCon) noexcept;

	// MARK: - Helpers

	/// Returns the graph's nodes.
	[[nodiscard]] std::expected<std::vector<AUNode>, OSStatus> Nodes() const noexcept;

	/// Returns a node's interactions.
	[[nodiscard]] std::expected<std::vector<AUNodeInteraction>, OSStatus> NodeInteractions(AUNode inNode) const noexcept;

	/// Returns the graph's nodes and their interactions.
	[[nodiscard]] std::expected<std::map<AUNode, std::vector<AUNodeInteraction>>, OSStatus> NodesAndInteractions() const noexcept;

	/// Returns the Audio Unit graph's latency.
	[[nodiscard]] std::expected<Float64, OSStatus> Latency() const noexcept;

	/// Returns the Audio Unit graph's tail time.
	[[nodiscard]] std::expected<Float64, OSStatus> TailTime() const noexcept;


	/// Returns the managed AUGraph object.
	[[nodiscard]] AUGraph _Nullable get() const noexcept;

	/// Replaces the managed AUGraph object with another AUGraph object.
	/// @note The object assumes responsibility for disposing of the passed AUGraph object using DisposeAUGraph.
	void reset(AUGraph _Nullable graph = nullptr) noexcept;

	/// Swaps the managed AUGraph object with the managed AUGraph object from another Audio Unit graph.
	void swap(CAAUGraph& other) noexcept;

	/// Releases ownership of the managed AUGraph object and returns it.
	/// @note The caller assumes responsibility for disposing of the returned AUGraph object using DisposeAUGraph.
	[[nodiscard]] AUGraph _Nullable release() noexcept;

private:
	/// The managed AUGraph object.
	AUGraph _Nullable graph_{nullptr};
};

// MARK: - Implementation -

inline CAAUGraph::operator bool() const noexcept
{
	return graph_ != nullptr;
}

inline CAAUGraph::operator AUGraph const _Nullable () const noexcept
{
	return graph_;
}

inline AUGraph _Nullable CAAUGraph::get() const noexcept
{
	return graph_;
}

inline void CAAUGraph::reset(AUGraph _Nullable graph) noexcept
{
	if(auto old = std::exchange(graph_, graph); old)
		DisposeAUGraph(old);
}

inline void CAAUGraph::swap(CAAUGraph& other) noexcept
{
	std::swap(graph_, other.graph_);
}

inline AUGraph _Nullable CAAUGraph::release() noexcept
{
	return std::exchange(graph_, nullptr);
}

} /* namespace CXXAudioToolbox */

CF_ASSUME_NONNULL_END
