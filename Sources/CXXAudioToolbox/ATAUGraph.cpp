//
// Copyright © 2021-2025 Stephen F. Booth
// Part of https://github.com/sbooth/CXXAudioToolbox
// MIT license
//

#import <exception>
#import <system_error>

#import "ATAUGraph.hpp"

namespace {

/// A std::error_category for return values from the AUGraph API.
class AUGraphErrorCategory : public std::error_category {
public:
	virtual const char * name() const noexcept override final { return "AUGraph"; }
	virtual std::string message(int condition) const override final
	{
		switch(static_cast<OSStatus>(condition)) {
				// CoreAudioBaseTypes.h
			case kAudio_NoError: 							return "The function call completed successfully";
			case kAudio_UnimplementedError: 				return "Unimplemented core routine";
			case kAudio_FileNotFoundError: 					return "File not found";
			case kAudio_FilePermissionError: 				return "File cannot be opened due to either file, directory, or sandbox permissions";
			case kAudio_TooManyFilesOpenError: 				return "File cannot be opened because too many files are already open";
			case kAudio_BadFilePathError: 					return "File cannot be opened because the specified path is malformed";
			case kAudio_ParamError: 						return "Error in user parameter list";
			case kAudio_MemFullError: 						return "Not enough room in heap zone";
				// AUGraph.h
			case kAUGraphErr_NodeNotFound: 					return "The specified node cannot be found";
			case kAUGraphErr_InvalidConnection: 			return "The attempted connection between two nodes cannot be made";
			case kAUGraphErr_OutputNodeErr: 				return "AUGraphs can only contain one OutputUnit";
			case kAUGraphErr_CannotDoInCurrentContext: 		return "The required lock is held by another thread";
			case kAUGraphErr_InvalidAudioUnit: 				return "The audio unit is invalid";
			default:										return "Unknown AUGraph error";
		}
	}
};

/// Global instance of AUGraph error category.
const AUGraphErrorCategory auGraphErrorCategory_;

/// Throws a std::system_error in the AUGraphErrorCategory if result != kAudio_NoError.
/// @param result An OSStatus result code.
/// @param operation An optional string describing the operation that produced result.
/// @throw std::system_error in the AUGraphErrorCategory.
inline void ThrowIfAUGraphError(OSStatus result, const char * const operation = nullptr)
{
	if(__builtin_expect(result != kAudio_NoError, false))
		throw std::system_error(result, auGraphErrorCategory_, operation);
}

/// A std::error_category for return values from the AudioUnit API.
class AudioUnitErrorCategory : public std::error_category {
public:
	virtual const char * name() const noexcept override final { return "AudioUnit"; }
	virtual std::string message(int condition) const override final
	{
		switch(static_cast<OSStatus>(condition)) {
				// CoreAudioBaseTypes.h
			case kAudio_NoError: 								return "The function call completed successfully";
			case kAudio_UnimplementedError: 					return "Unimplemented core routine";
			case kAudio_FileNotFoundError: 						return "File not found";
			case kAudio_FilePermissionError: 					return "File cannot be opened due to either file, directory, or sandbox permissions";
			case kAudio_TooManyFilesOpenError: 					return "File cannot be opened because too many files are already open";
			case kAudio_BadFilePathError: 						return "File cannot be opened because the specified path is malformed";
			case kAudio_ParamError: 							return "Error in user parameter list";
			case kAudio_MemFullError: 							return "Not enough room in heap zone";
				// AUComponent.h
			case kAudioUnitErr_InvalidProperty: 				return "The property is not supported";
			case kAudioUnitErr_InvalidParameter: 				return "The parameter is not supported";
			case kAudioUnitErr_InvalidElement: 					return "The specified element is not valid";
			case kAudioUnitErr_NoConnection: 					return "There is no connection (generally an audio unit is asked to render but it has not input from which to gather data)";
			case kAudioUnitErr_FailedInitialization: 			return "The audio unit is unable to be initialized";
			case kAudioUnitErr_TooManyFramesToProcess: 			return "When an audio unit is initialized it has a value which specifies the max number of frames it will be asked to render at any given time. If an audio unit is asked to render more than this, this error is returned";
			case kAudioUnitErr_InvalidFile: 					return "If an audio unit uses external files as a data source, this error is returned if a file is invalid (Apple's DLS synth returns this error)";
			case kAudioUnitErr_UnknownFileType: 				return "If an audio unit uses external files as a data source, this error is returned if a file is invalid (Apple's DLS synth returns this error)";
			case kAudioUnitErr_FileNotSpecified: 				return "If an audio unit uses external files as a data source, this error is returned if a file hasn't been set on it (Apple's DLS synth returns this error)";
			case kAudioUnitErr_FormatNotSupported: 				return "Returned if an input or output format is not supported";
			case kAudioUnitErr_Uninitialized: 					return "Returned if an operation requires an audio unit to be initialized and it is not";
			case kAudioUnitErr_InvalidScope: 					return "The specified scope is invalid";
			case kAudioUnitErr_PropertyNotWritable: 			return "The property cannot be written";
			case kAudioUnitErr_CannotDoInCurrentContext: 		return "Returned when an audio unit is in a state where it can't perform the requested action now - but it could later. It's usually used to guard a render operation when a reconfiguration of its internal state is being performed";
			case kAudioUnitErr_InvalidPropertyValue: 			return "The property is valid, but the value of the property being provided is not";
			case kAudioUnitErr_PropertyNotInUse: 				return "Returned when a property is valid, but it hasn't been set to a valid value at this time";
			case kAudioUnitErr_Initialized: 					return "Indicates the operation cannot be performed because the audio unit is initialized";
			case kAudioUnitErr_InvalidOfflineRender: 			return "Used to indicate that the offline render operation is invalid. For instance, when the audio unit needs to be pre-flighted, but it hasn't been";
			case kAudioUnitErr_Unauthorized: 					return "Returned by either Open or Initialize, this error is used to indicate that the audio unit is not authorised, that it cannot be used. A host can then present a UI to notify the user the audio unit is not able to be used in its current state";
			case kAudioUnitErr_MIDIOutputBufferFull: 			return "Returned during the render call, if the audio unit produces more MIDI output, than the default allocated buffer. The audio unit can provide a size hint, in case it needs a larger buffer. See the documentation for AUAudioUnit's MIDIOutputBufferSizeHint property";
			case kAudioComponentErr_InstanceTimedOut: 			return "kAudioComponentErr_InstanceTimedOut";
			case kAudioComponentErr_InstanceInvalidated: 		return "The component instance's implementation is not available, most likely because the process that published it is no longer running";
			case kAudioUnitErr_RenderTimeout: 					return "The audio unit did not satisfy the render request in time";
			case kAudioUnitErr_ExtensionNotFound: 				return "The specified identifier did not match any Audio Unit Extensions";
			case kAudioUnitErr_InvalidParameterValue: 			return "The parameter value is not supported, e.g. the value specified is NaN or infinite";
			case kAudioUnitErr_InvalidFilePath: 				return "The file path that was passed is not supported. It is either too long or contains invalid characters";
			case kAudioUnitErr_MissingKey: 						return "A required key is missing from a dictionary object";
			case kAudioComponentErr_DuplicateDescription: 		return "A non-unique component description was provided to AudioOutputUnitPublish";
			case kAudioComponentErr_UnsupportedType: 			return "An unsupported component type was provided to AudioOutputUnitPublish";
			case kAudioComponentErr_TooManyInstances: 			return "Components published via AudioOutputUnitPublish may only have one instance";
			case kAudioComponentErr_NotPermitted: 				return "App needs \"inter-app-audio\" entitlement or host app needs \"audio\" in its UIBackgroundModes. Or app is trying to register a component not declared in its Info.plist";
			case kAudioComponentErr_InitializationTimedOut: 	return "Host did not render in a timely manner; must uninitialize and reinitialize";
			case kAudioComponentErr_InvalidFormat: 				return "Inter-app AU element formats must have sample rates matching the hardware";
			default:											return "Unknown AudioUnit error";
		}
	}
};

/// Global instance of AudioUnit error category.
const AudioUnitErrorCategory audioUnitErrorCategory_;

/// Throws a std::system_error in the AudioUnitErrorCategory if result != kAudio_NoError.
/// @param result An OSStatus result code.
/// @param operation An optional string describing the operation that produced result.
/// @throw std::system_error in the AudioUnitErrorCategory.
inline void ThrowIfAudioUnitError(OSStatus result, const char * const operation = nullptr)
{
	if(__builtin_expect(result != kAudio_NoError, false))
		throw std::system_error(result, audioUnitErrorCategory_, operation);
}

} /* namespace */

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
