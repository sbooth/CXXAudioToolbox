//
// Copyright © 2021-2025 Stephen F. Booth
// Part of https://github.com/sbooth/CXXAudioToolbox
// MIT license
//

#import <utility>

#import "CAAudioFile.hpp"
#import "AudioToolboxErrors.hpp"
#import "StringFormatting.hpp"

CXXAudioToolbox::CAAudioFile::CAAudioFile(CAAudioFile&& rhs) noexcept
: mAudioFileID{std::exchange(rhs.mAudioFileID, nullptr)}
{}

CXXAudioToolbox::CAAudioFile& CXXAudioToolbox::CAAudioFile::operator=(CAAudioFile&& rhs) noexcept
{
	if(this != &rhs) {
		if(mAudioFileID)
			AudioFileClose(mAudioFileID);
		mAudioFileID = std::exchange(rhs.mAudioFileID, nullptr);
	}
	return *this;
}

/// Destroys the audio file and releases all associated resources.
CXXAudioToolbox::CAAudioFile::~CAAudioFile() noexcept
{
	if(mAudioFileID)
		AudioFileClose(mAudioFileID);
}

void CXXAudioToolbox::CAAudioFile::OpenURL(CFURLRef inURL, AudioFilePermissions inPermissions, AudioFileTypeID inFileTypeHint)
{
	Close();
	const auto result = AudioFileOpenURL(inURL, inPermissions, inFileTypeHint, &mAudioFileID);
	ThrowIfAudioFileError(result, [=]() {
		return concat({
			"AudioFileOpenURL(",
			string_from_cftype(CFURLGetString(inURL)),
			"0x", to_hex_string(static_cast<uint8_t>(inPermissions)),
			", ", to_fourcc_string(inFileTypeHint),
			")",
			" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
		});
	});
}

void CXXAudioToolbox::CAAudioFile::CreateWithURL(CFURLRef inURL, AudioFileTypeID inFileType, const AudioStreamBasicDescription& inFormat, AudioFileFlags inFlags)
{
	Close();
	const auto result = AudioFileCreateWithURL(inURL, inFileType, &inFormat, inFlags, &mAudioFileID);
	ThrowIfAudioFileError(result, [=]() {
		return concat({
			"AudioFileCreateWithURL(",
			string_from_cftype(CFURLGetString(inURL)),
			", ", to_fourcc_string(inFileType),
			", ", to_fourcc_string(inFormat.mFormatID),
			", 0x", to_hex_string(inFlags), 
			")",
			" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
		});
	});
}

void CXXAudioToolbox::CAAudioFile::InitializeWithCallbacks(void *inClientData, AudioFile_ReadProc inReadFunc, AudioFile_WriteProc inWriteFunc, AudioFile_GetSizeProc inGetSizeFunc, AudioFile_SetSizeProc inSetSizeFunc, AudioFileTypeID inFileType, const AudioStreamBasicDescription& inFormat, AudioFileFlags inFlags)
{
	Close();
	const auto result = AudioFileInitializeWithCallbacks(inClientData, inReadFunc, inWriteFunc, inGetSizeFunc, inSetSizeFunc, inFileType, &inFormat, inFlags, &mAudioFileID);
	ThrowIfAudioFileError(result, [=]() {
		return concat({
			"AudioFileInitializeWithCallbacks("
			"0x", to_hex_string(reinterpret_cast<uintptr_t>(inClientData)),
			", _, _, _, _"
			", ", to_fourcc_string(inFileType),
			", ", to_fourcc_string(inFormat.mFormatID),
			", 0x", to_hex_string(inFlags),
			")",
			" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
		});
	});
}

void CXXAudioToolbox::CAAudioFile::OpenWithCallbacks(void *inClientData, AudioFile_ReadProc inReadFunc, AudioFile_WriteProc _Nullable inWriteFunc, AudioFile_GetSizeProc inGetSizeFunc, AudioFile_SetSizeProc _Nullable inSetSizeFunc, AudioFileTypeID inFileTypeHint)
{
	Close();
	const auto result = AudioFileOpenWithCallbacks(inClientData, inReadFunc, inWriteFunc, inGetSizeFunc, inSetSizeFunc, inFileTypeHint, &mAudioFileID);
	ThrowIfAudioFileError(result, [=]() {
		return concat({
			"AudioFileOpenWithCallbacks("
			"0x", to_hex_string(reinterpret_cast<uintptr_t>(inClientData)),
			", _, _, _, _"
			", ", to_fourcc_string(inFileTypeHint),
			")",
			" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
		});
	});
}

void CXXAudioToolbox::CAAudioFile::Close()
{
	if(mAudioFileID) {
		const auto result = AudioFileClose(mAudioFileID);
		mAudioFileID = nullptr;
		ThrowIfAudioFileError(result, [=]() {
			return concat({
				"AudioFileClose"
				" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
			});
		});
	}
}

void CXXAudioToolbox::CAAudioFile::Optimize()
{
	const auto result = AudioFileOptimize(mAudioFileID);
	ThrowIfAudioFileError(result, [=]() {
		return concat({
			"AudioFileOptimize"
			" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
		});
	});
}

OSStatus CXXAudioToolbox::CAAudioFile::ReadBytes(bool inUseCache, SInt64 inStartingByte, UInt32& ioNumBytes, void *outBuffer)
{
	const auto result = AudioFileReadBytes(mAudioFileID, inUseCache, inStartingByte, &ioNumBytes, outBuffer);
	switch(result) {
		case noErr:
		case kAudioFileEndOfFileError:
			break;
		default:
			ThrowIfAudioFileError(result, [=]() {
				return concat({
					"AudioFileReadBytes(",
					inUseCache ? "true" : "false"
					", ", to_string(inStartingByte),
					", ", to_string(ioNumBytes),
					", 0x", to_hex_string(reinterpret_cast<uintptr_t>(outBuffer)),
					")",
					" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
				});
			});
			break;
	}
	return result;
}

void CXXAudioToolbox::CAAudioFile::WriteBytes(bool inUseCache, SInt64 inStartingByte, UInt32& ioNumBytes, const void *inBuffer)
{
	const auto result = AudioFileWriteBytes(mAudioFileID, inUseCache, inStartingByte, &ioNumBytes, inBuffer);
	ThrowIfAudioFileError(result, [=]() {
		return concat({
			"AudioFileWriteBytes(",
			inUseCache ? "true" : "false"
			", ", to_string(inStartingByte),
			", ", to_string(ioNumBytes),
			", 0x", to_hex_string(reinterpret_cast<uintptr_t>(inBuffer)),
			")",
			" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
		});
	});
}

OSStatus CXXAudioToolbox::CAAudioFile::ReadPacketData(bool inUseCache, UInt32& ioNumBytes, AudioStreamPacketDescription * _Nullable outPacketDescriptions, SInt64 inStartingPacket, UInt32& ioNumPackets, void * _Nullable outBuffer)
{
	const auto result = AudioFileReadPacketData(mAudioFileID, inUseCache, &ioNumBytes, outPacketDescriptions, inStartingPacket, &ioNumPackets, outBuffer);
	switch(result) {
		case noErr:
		case kAudioFileEndOfFileError:
			break;
		default:
			ThrowIfAudioFileError(result, [=]() {
				return concat({
					"AudioFileReadPacketData(",
					inUseCache ? "true" : "false"
					", ", to_string(ioNumBytes),
					", 0x", to_hex_string(reinterpret_cast<uintptr_t>(outPacketDescriptions)),
					", ", to_string(inStartingPacket),
					", ", to_string(ioNumPackets),
					", 0x", to_hex_string(reinterpret_cast<uintptr_t>(outBuffer)),
					")",
					" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
				});
			});
			break;
	}
	return result;
}

void CXXAudioToolbox::CAAudioFile::WritePackets(bool inUseCache, UInt32 inNumBytes, const AudioStreamPacketDescription * _Nullable inPacketDescriptions, SInt64 inStartingPacket, UInt32& ioNumPackets, const void *inBuffer)
{
	const auto result = AudioFileWritePackets(mAudioFileID, inUseCache, inNumBytes, inPacketDescriptions, inStartingPacket, &ioNumPackets, inBuffer);
	ThrowIfAudioFileError(result, [=]() {
		return concat({
			"AudioFileWritePackets(", 
			inUseCache ? "true" : "false"
			", ", to_string(inNumBytes),
			", 0x", to_hex_string(reinterpret_cast<uintptr_t>(inPacketDescriptions)),
			", ", to_string(inStartingPacket),
			", ", to_string(ioNumPackets),
			", 0x", to_hex_string(reinterpret_cast<uintptr_t>(inBuffer)),
			")",
			" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
		});
	});
}

UInt32 CXXAudioToolbox::CAAudioFile::GetUserDataSize(UInt32 inUserDataID, UInt32 inIndex)
{
	UInt32 size;
	const auto result = AudioFileGetUserDataSize(mAudioFileID, inUserDataID, inIndex, &size);
	ThrowIfAudioFileError(result, [=]() {
		return concat({
			"AudioFileGetUserDataSize(",
			to_fourcc_string(inUserDataID),
			", ", to_string(inIndex),
			")",
			" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
		});
	});
	return size;
}

void CXXAudioToolbox::CAAudioFile::GetUserData(UInt32 inUserDataID, UInt32 inIndex, UInt32& ioUserDataSize, void *outUserData) const
{
	const auto result = AudioFileGetUserData(mAudioFileID, inUserDataID, inIndex, &ioUserDataSize, outUserData);
	ThrowIfAudioFileError(result, [=]() {
		return concat({
			"AudioFileGetUserData(",
			to_fourcc_string(inUserDataID),
			", ", to_string(inIndex),
			", ", to_string(ioUserDataSize),
			", 0x", to_hex_string(reinterpret_cast<uintptr_t>(outUserData)),
			")",
			" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
		});
	});
}

void CXXAudioToolbox::CAAudioFile::SetUserData(UInt32 inUserDataID, UInt32 inIndex, UInt32 inUserDataSize, const void *inUserData)
{
	const auto result = AudioFileSetUserData(mAudioFileID, inUserDataID, inIndex, inUserDataSize, inUserData);
	ThrowIfAudioFileError(result, [=]() {
		return concat({
			"SetUserData(",
			to_fourcc_string(inUserDataID),
			", ", to_string(inIndex),
			", ", to_string(inUserDataSize),
			", 0x", to_hex_string(reinterpret_cast<uintptr_t>(inUserData)),
			")",
			" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
		});
	});
}

void CXXAudioToolbox::CAAudioFile::RemoveUserData(UInt32 inUserDataID, UInt32 inIndex)
{
	const auto result = AudioFileRemoveUserData(mAudioFileID, inUserDataID, inIndex);
	ThrowIfAudioFileError(result, [=]() {
		return concat({
			"AudioFileRemoveUserData(",
			to_fourcc_string(inUserDataID),
			", ", to_string(inIndex),
			")",
			" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
		});
	});
}

UInt32 CXXAudioToolbox::CAAudioFile::GetPropertyInfo(AudioFilePropertyID inPropertyID, UInt32 * _Nullable isWritable) const
{
	UInt32 size;
	const auto result = AudioFileGetPropertyInfo(mAudioFileID, inPropertyID, &size, isWritable);
	ThrowIfAudioFileError(result, [=]() {
		return concat({
			"AudioFileGetPropertyInfo(",
			to_fourcc_string(inPropertyID),
			")",
			" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
		});
	});
	return size;
}

void CXXAudioToolbox::CAAudioFile::GetProperty(AudioFilePropertyID inPropertyID, UInt32& ioDataSize, void *outPropertyData) const
{
	const auto result = AudioFileGetProperty(mAudioFileID, inPropertyID, &ioDataSize, outPropertyData);
	ThrowIfAudioFileError(result, [=]() {
		return concat({
			"AudioFileGetProperty(", to_fourcc_string(inPropertyID),
			", ", to_string(ioDataSize),
			", 0x", to_hex_string(reinterpret_cast<uintptr_t>(outPropertyData)),
			")",
			" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
		});
	});
}

void CXXAudioToolbox::CAAudioFile::SetProperty(AudioFilePropertyID inPropertyID, UInt32 inDataSize, const void *inPropertyData)
{
	const auto result = AudioFileSetProperty(mAudioFileID, inPropertyID, inDataSize, inPropertyData);
	ThrowIfAudioFileError(result, [=]() {
		return concat({
			"AudioFileSetProperty(",
			to_fourcc_string(inPropertyID),
			", ", to_string(inDataSize),
			", 0x", to_hex_string(reinterpret_cast<uintptr_t>(inPropertyData)),
			")",
			" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
		});
	});
}

AudioFileTypeID CXXAudioToolbox::CAAudioFile::FileFormat() const
{
	AudioFileTypeID fileFormat;
	UInt32 size = sizeof(fileFormat);
	GetProperty(kAudioFilePropertyFileFormat, size, &fileFormat);
	return fileFormat;
}

CXXCoreAudio::CAStreamDescription CXXAudioToolbox::CAAudioFile::DataFormat() const
{
	CXXCoreAudio::CAStreamDescription fileDataFormat;
	UInt32 size = sizeof(fileDataFormat);
	GetProperty(kAudioFilePropertyDataFormat, size, &fileDataFormat);
	return fileDataFormat;
}

// MARK: Global Properties

UInt32 CXXAudioToolbox::CAAudioFile::GetGlobalInfoSize(AudioFilePropertyID inPropertyID, UInt32 inSpecifierSize, void * _Nullable inSpecifier)
{
	UInt32 size;
	const auto result = AudioFileGetGlobalInfoSize(inPropertyID, inSpecifierSize, inSpecifier, &size);
	ThrowIfAudioFileError(result, [=]() {
		return concat({
			"AudioFileGetGlobalInfoSize(", 
			to_fourcc_string(inPropertyID),
			", ", to_string(inSpecifierSize),
			", 0x", to_hex_string(reinterpret_cast<uintptr_t>(inSpecifier)),
			")",
			" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
		});
	});
	return size;
}

void CXXAudioToolbox::CAAudioFile::GetGlobalInfo(AudioFilePropertyID inPropertyID, UInt32 inSpecifierSize, void * _Nullable inSpecifier, UInt32& ioDataSize, void *outPropertyData)
{
	const auto result = AudioFileGetGlobalInfo(inPropertyID, inSpecifierSize, inSpecifier, &ioDataSize, outPropertyData);
	ThrowIfAudioFileError(result, [=]() {
		return concat({
			"AudioFileGetGlobalInfo(",
			to_fourcc_string(inPropertyID),
			", ", to_string(inSpecifierSize),
			", 0x", to_hex_string(reinterpret_cast<uintptr_t>(inSpecifier)),
			", ", to_string(ioDataSize),
			", 0x", to_hex_string(reinterpret_cast<uintptr_t>(outPropertyData)),
			")",
			" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
		});
	});
}

std::vector<AudioFileTypeID> CXXAudioToolbox::CAAudioFile::ReadableTypes()
{
	auto size = GetGlobalInfoSize(kAudioFileGlobalInfo_ReadableTypes, 0, nullptr);
	auto count = size / sizeof(AudioFileTypeID);
	auto types = std::vector<AudioFileTypeID>(count);
	GetGlobalInfo(kAudioFileGlobalInfo_ReadableTypes, 0, nullptr, size, types.data());
	return types;
}

std::vector<AudioFileTypeID> CXXAudioToolbox::CAAudioFile::WritableTypes()
{
	auto size = GetGlobalInfoSize(kAudioFileGlobalInfo_WritableTypes, 0, nullptr);
	auto count = size / sizeof(AudioFileTypeID);
	auto types = std::vector<AudioFileTypeID>(count);
	GetGlobalInfo(kAudioFileGlobalInfo_WritableTypes, 0, nullptr, size, types.data());
	return types;
}

CFStringRef CXXAudioToolbox::CAAudioFile::CopyFileTypeName(AudioFileTypeID type)
{
	CFStringRef s;
	UInt32 size = sizeof(s);
	GetGlobalInfo(kAudioFileGlobalInfo_FileTypeName, sizeof(type), &type, size, &s);
	return s;
}

std::vector<CXXCoreAudio::CAStreamDescription> CXXAudioToolbox::CAAudioFile::AvailableStreamDescriptions(AudioFileTypeID fileType, AudioFormatID formatID)
{
	AudioFileTypeAndFormatID spec{ fileType, formatID };
	auto size = GetGlobalInfoSize(kAudioFileGlobalInfo_AvailableStreamDescriptionsForFormat, sizeof(spec), &spec);
	auto count = size / sizeof(AudioStreamBasicDescription);
	auto streamDescriptions = std::vector<CXXCoreAudio::CAStreamDescription>(count);
	GetGlobalInfo(kAudioFileGlobalInfo_AvailableStreamDescriptionsForFormat, sizeof(spec), &spec, size, streamDescriptions.data());
	return streamDescriptions;
}

std::vector<AudioFormatID> CXXAudioToolbox::CAAudioFile::AvailableFormatIDs(AudioFileTypeID type)
{
	auto size = GetGlobalInfoSize(kAudioFileGlobalInfo_AvailableFormatIDs, sizeof(type), &type);
	auto count = size / sizeof(AudioFormatID);
	auto formatIDs = std::vector<AudioFormatID>(count);
	GetGlobalInfo(kAudioFileGlobalInfo_AvailableFormatIDs, sizeof(type), &type, size, formatIDs.data());
	return formatIDs;
}


CFArrayRef CXXAudioToolbox::CAAudioFile::CopyAllExtensions()
{
	CFArrayRef a;
	UInt32 size = sizeof(a);
	GetGlobalInfo(kAudioFileGlobalInfo_AllExtensions, 0, nullptr, size, &a);
	return a;
}

CFArrayRef CXXAudioToolbox::CAAudioFile::CopyAllUTIs()
{
	CFArrayRef a;
	UInt32 size = sizeof(a);
	GetGlobalInfo(kAudioFileGlobalInfo_AllUTIs, 0, nullptr, size, &a);
	return a;
}

CFArrayRef CXXAudioToolbox::CAAudioFile::CopyAllMIMETypes()
{
	CFArrayRef a;
	UInt32 size = sizeof(a);
	GetGlobalInfo(kAudioFileGlobalInfo_AllMIMETypes, 0, nullptr, size, &a);
	return a;
}


CFArrayRef CXXAudioToolbox::CAAudioFile::CopyExtensionsForType(AudioFileTypeID type)
{
	CFArrayRef a;
	UInt32 size = sizeof(a);
	GetGlobalInfo(kAudioFileGlobalInfo_ExtensionsForType, sizeof(type), &type, size, &a);
	return a;
}

CFArrayRef CXXAudioToolbox::CAAudioFile::CopyUTIsForType(AudioFileTypeID type)
{
	CFArrayRef a;
	UInt32 size = sizeof(a);
	GetGlobalInfo(kAudioFileGlobalInfo_UTIsForType, sizeof(type), &type, size, &a);
	return a;
}

CFArrayRef CXXAudioToolbox::CAAudioFile::CopyMIMETypesForType(AudioFileTypeID type)
{
	CFArrayRef mimeTypes;
	UInt32 size = sizeof(mimeTypes);
	GetGlobalInfo(kAudioFileGlobalInfo_MIMETypesForType, sizeof(type), &type, size, &mimeTypes);
	return mimeTypes;
}


std::vector<AudioFileTypeID> CXXAudioToolbox::CAAudioFile::TypesForMIMEType(CFStringRef mimeType)
{
	auto size = GetGlobalInfoSize(kAudioFileGlobalInfo_TypesForMIMEType, sizeof(mimeType), const_cast<void *>(reinterpret_cast<const void *>(mimeType)));
	auto count = size / sizeof(AudioFileTypeID);
	auto types = std::vector<AudioFileTypeID>(count);
	GetGlobalInfo(kAudioFileGlobalInfo_TypesForMIMEType, sizeof(mimeType), const_cast<void *>(reinterpret_cast<const void *>(mimeType)), size, types.data());
	return types;
}

std::vector<AudioFileTypeID> CXXAudioToolbox::CAAudioFile::TypesForUTI(CFStringRef uti)
{
	auto size = GetGlobalInfoSize(kAudioFileGlobalInfo_TypesForUTI, sizeof(uti), const_cast<void *>(reinterpret_cast<const void *>(uti)));
	auto count = size / sizeof(AudioFileTypeID);
	auto types = std::vector<AudioFileTypeID>(count);
	GetGlobalInfo(kAudioFileGlobalInfo_TypesForUTI, sizeof(uti), const_cast<void *>(reinterpret_cast<const void *>(uti)), size, types.data());
	return types;
}

std::vector<AudioFileTypeID> CXXAudioToolbox::CAAudioFile::TypesForExtension(CFStringRef extension)
{
	auto size = GetGlobalInfoSize(kAudioFileGlobalInfo_TypesForExtension, sizeof(extension), const_cast<void *>(reinterpret_cast<const void *>(extension)));
	auto count = size / sizeof(AudioFileTypeID);
	auto types = std::vector<AudioFileTypeID>(count);
	GetGlobalInfo(kAudioFileGlobalInfo_TypesForExtension, sizeof(extension), const_cast<void *>(reinterpret_cast<const void *>(extension)), size, types.data());
	return types;
}
