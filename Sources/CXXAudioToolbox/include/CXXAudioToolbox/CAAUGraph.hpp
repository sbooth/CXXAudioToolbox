//
// Copyright © 2021-2025 Stephen F. Booth
// Part of https://github.com/sbooth/CXXAudioToolbox
// MIT license
//

#pragma once

#import <map>
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

	/// Returns true if this object's internal AUGraph object is not null.
	explicit operator bool() const noexcept
	{
		return auGraph_ != nullptr;
	}

	/// Returns the object's internal AUGraph object.
	operator AUGraph const _Nullable () const noexcept
	{
		return auGraph_;
	}

	/// Creates a new Audio Unit graph.
	/// @throw std::system_error.
	void New();

	/// Disposes the Audio Unit graph.
	/// @throw std::system_error.
	void Dispose();

	// MARK: - Node State

	/// Adds a node to the Audio Unit graph.
	/// @throw std::system_error.
	AUNode AddNode(const AudioComponentDescription *inDescription);

	/// Removes a node from the Audio Unit graph.
	/// @throw std::system_error.
	void RemoveNode(AUNode inNode);

	/// Returns the number of nodes in the Audio Unit graph.
	/// @throw std::system_error.
	UInt32 GetNodeCount() const;

	/// Returns the node at a given index
	/// @throw std::system_error.
	AUNode GetIndNode(UInt32 inIndex) const;

	/// Returns information about a particular AUNode.
	/// @throw std::system_error.
	void NodeInfo(AUNode inNode, AudioComponentDescription * _Nullable outDescription, AudioUnit _Nullable * _Nullable outAudioUnit) const;

#if !TARGET_OS_IPHONE
	// MARK: - Sub Graphs

	/// Creates a node that will represent a sub graph.
	/// @throw std::system_error.
	AUNode NewNodeSubGraph();

	/// Returns the sub graph represented by a particular AUNode.
	/// @throw std::system_error.
	AUGraph GetNodeInfoSubGraph(AUNode inNode) const;

	/// Returns true if the node represents a sub graph.
	/// @throw std::system_error.
	bool IsNodeSubGraph(AUNode inNode) const;
#endif /* !TARGET_OS_IPHONE */

	// MARK: - Node Interactions

	/// Connects a node's output to a node's input.
	/// @throw std::system_error.
	void ConnectNodeInput(AUNode inSourceNode, UInt32 inSourceOutputNumber, AUNode inDestNode, UInt32 inDestInputNumber);

	/// Sets a callback for the specified node's specified input.
	/// @throw std::system_error.
	void SetNodeInputCallback(AUNode inDestNode, UInt32 inDestInputNumber, const AURenderCallbackStruct *inInputCallback);

	/// Disconnects a node's input.
	/// @throw std::system_error.
	void DisconnectNodeInput(AUNode inDestNode, UInt32 inDestInputNumber);

	/// Clears all of the interactions in a graph.
	/// @throw std::system_error.
	void ClearConnections();

	/// Returns the number of interactions in the Audio Unit graph.
	/// @throw std::system_error.
	UInt32 GetNumberOfInteractions() const;

	/// Returns information about a particular interaction in a graph.
	/// @throw std::system_error.
	AUNodeInteraction GetInteractionInfo(UInt32 inInteractionIndex) const;

	/// Returns the number of interactions of a graph's node.
	/// @throw std::system_error.
	UInt32 CountNodeInteractions(AUNode inNode) const;

	/// Retrieves information about the interactions in a graph for a given node.
	/// @throw std::system_error.
	void GetNodeInteractions(AUNode inNode, UInt32 *ioNumInteractions, AUNodeInteraction *outInteractions) const;

	// MARK: -

	/// Updates the state of a running AUGraph.
	/// @throw std::system_error.
	bool Update();

	// MARK: - State Management

	/// Opens a graph.
	/// @throw std::system_error.
	void Open();

	/// Closes a graph.
	/// @throw std::system_error.
	void Close();

	/// Initializes a graph.
	/// @throw std::system_error.
	void Initialize();

	/// Uninitialize a graph.
	/// @throw std::system_error.
	void Uninitialize();

	/// Starts a graph.
	/// @throw std::system_error.
	void Start();

	/// Stops a graph.
	/// @throw std::system_error.
	void Stop();

	/// Returns true if the Audio Unit graph is open.
	/// @throw std::system_error.
	bool IsOpen() const;

	/// Returns true if the Audio Unit graph is initialized.
	/// @throw std::system_error.
	bool IsInitialized() const;

	/// Returns true if the Audio Unit graph is running.
	/// @throw std::system_error.
	bool IsRunning() const;

	// MARK: - Utilities

	/// Returns a short-term running average of the current CPU load of the graph.
	/// @throw std::system_error.
	Float32 GetCPULoad() const;

	/// Returns the max CPU load of the graph since this call was last made or the graph was last started.
	/// @throw std::system_error.
	Float32 GetMaxCPULoad() const;

	/// Adds a notification callback.
	/// @throw std::system_error.
	void AddRenderNotify(AURenderCallback inCallback, void * _Nullable inRefCon);

	/// Removes a notification callback.
	/// @throw std::system_error.
	void RemoveRenderNotify(AURenderCallback inCallback, void * _Nullable inRefCon);

	// MARK: - Helpers

	/// Returns the graph's nodes.
	/// @throw std::system_error.
	std::vector<AUNode> Nodes() const;

	/// Returns a node's interactions.
	/// @throw std::system_error.
	std::vector<AUNodeInteraction> NodeInteractions(AUNode inNode) const;

	/// Returns the graph's nodes and their interactions.
	/// @throw std::system_error.
	std::map<AUNode, std::vector<AUNodeInteraction>> NodesAndInteractions() const;

	/// Returns the Audio Unit graph's latency.
	/// @throw std::system_error..
	Float64 Latency() const;

	/// Returns the Audio Unit graph's tail time.
	/// @throw std::system_error.
	Float64 TailTime() const;

private:
	/// The underlying AUGraph object.
	AUGraph _Nullable auGraph_{nullptr};
};

} /* namespace CXXAudioToolbox */

CF_ASSUME_NONNULL_END
