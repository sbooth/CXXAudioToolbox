//
// SPDX-FileCopyrightText: 2021 Stephen F. Booth <contact@sbooth.dev>
// SPDX-License-Identifier: MIT
//
// Part of https://github.com/sbooth/CXXAudioToolbox
//

#import "CAAudioFile.hpp"

CXXAudioToolbox::CAAudioFile::CAAudioFile(CAAudioFile&& other) noexcept
: audioFile_{other.release()}
{}

CXXAudioToolbox::CAAudioFile& CXXAudioToolbox::CAAudioFile::operator=(CAAudioFile&& other) noexcept
{
	reset(other.release());
	return *this;
}

/// Destroys the audio file and releases all associated resources.
CXXAudioToolbox::CAAudioFile::~CAAudioFile() noexcept
{
	reset();
}

std::expected<void, OSStatus> CXXAudioToolbox::CAAudioFile::OpenURL(CFURLRef inURL, AudioFilePermissions inPermissions, AudioFileTypeID inFileTypeHint) noexcept
{
	const auto audio_file_open_url = [&] -> std::expected<void, OSStatus> {
		const auto result = AudioFileOpenURL(inURL, inPermissions, inFileTypeHint, &audioFile_);
		if(result != noErr)
			return std::unexpected(result);
		return {};
	};

	return Close().and_then(audio_file_open_url);
}

std::expected<void, OSStatus> CXXAudioToolbox::CAAudioFile::CreateWithURL(CFURLRef inURL, AudioFileTypeID inFileType, const AudioStreamBasicDescription& inFormat, AudioFileFlags inFlags) noexcept
{
	const auto audio_file_create_with_url = [&] -> std::expected<void, OSStatus> {
		const auto result = AudioFileCreateWithURL(inURL, inFileType, &inFormat, inFlags, &audioFile_);
		if(result != noErr)
			return std::unexpected(result);
		return {};
	};

	return Close().and_then(audio_file_create_with_url);
}

std::expected<void, OSStatus> CXXAudioToolbox::CAAudioFile::InitializeWithCallbacks(void *inClientData, AudioFile_ReadProc inReadFunc, AudioFile_WriteProc inWriteFunc, AudioFile_GetSizeProc inGetSizeFunc, AudioFile_SetSizeProc inSetSizeFunc, AudioFileTypeID inFileType, const AudioStreamBasicDescription& inFormat, AudioFileFlags inFlags) noexcept
{
	const auto audio_file_initialize_with_callbacks = [&] -> std::expected<void, OSStatus> {
		const auto result = AudioFileInitializeWithCallbacks(inClientData, inReadFunc, inWriteFunc, inGetSizeFunc, inSetSizeFunc, inFileType, &inFormat, inFlags, &audioFile_);
		if(result != noErr)
			return std::unexpected(result);
		return {};
	};

	return Close().and_then(audio_file_initialize_with_callbacks);
}

std::expected<void, OSStatus> CXXAudioToolbox::CAAudioFile::OpenWithCallbacks(void *inClientData, AudioFile_ReadProc inReadFunc, AudioFile_WriteProc _Nullable inWriteFunc, AudioFile_GetSizeProc inGetSizeFunc, AudioFile_SetSizeProc _Nullable inSetSizeFunc, AudioFileTypeID inFileTypeHint) noexcept
{
	const auto audio_file_openw_with_callbacks = [&] -> std::expected<void, OSStatus> {
		const auto result = AudioFileOpenWithCallbacks(inClientData, inReadFunc, inWriteFunc, inGetSizeFunc, inSetSizeFunc, inFileTypeHint, &audioFile_);
		if(result != noErr)
			return std::unexpected(result);
		return {};
	};

	return Close().and_then(audio_file_openw_with_callbacks);
}

std::expected<void, OSStatus> CXXAudioToolbox::CAAudioFile::Close() noexcept
{
	if(audioFile_) {
		const auto result = AudioFileClose(audioFile_);
		audioFile_ = nullptr;
		if(result != noErr)
			return std::unexpected(result);
	}
	return {};
}

std::expected<void, OSStatus> CXXAudioToolbox::CAAudioFile::Optimize() noexcept
{
	const auto result = AudioFileOptimize(audioFile_);
	if(result != noErr)
		return std::unexpected(result);
	return {};
}

std::expected<OSStatus, OSStatus> CXXAudioToolbox::CAAudioFile::ReadBytes(bool inUseCache, SInt64 inStartingByte, UInt32& ioNumBytes, void *outBuffer) noexcept
{
	const auto result = AudioFileReadBytes(audioFile_, inUseCache, inStartingByte, &ioNumBytes, outBuffer);
	switch(result) {
		case noErr:
		case kAudioFileEndOfFileError:
			return result;
		default:
			return std::unexpected(result);
	}
}

std::expected<void, OSStatus> CXXAudioToolbox::CAAudioFile::WriteBytes(bool inUseCache, SInt64 inStartingByte, UInt32& ioNumBytes, const void *inBuffer) noexcept
{
	const auto result = AudioFileWriteBytes(audioFile_, inUseCache, inStartingByte, &ioNumBytes, inBuffer);
	if(result != noErr)
		return std::unexpected(result);
	return {};
}

std::expected<OSStatus, OSStatus> CXXAudioToolbox::CAAudioFile::ReadPacketData(bool inUseCache, UInt32& ioNumBytes, AudioStreamPacketDescription * _Nullable outPacketDescriptions, SInt64 inStartingPacket, UInt32& ioNumPackets, void * _Nullable outBuffer) noexcept
{
	const auto result = AudioFileReadPacketData(audioFile_, inUseCache, &ioNumBytes, outPacketDescriptions, inStartingPacket, &ioNumPackets, outBuffer);
	switch(result) {
		case noErr:
		case kAudioFileEndOfFileError:
			return result;
		default:
			return std::unexpected(result);
	}
}

std::expected<void, OSStatus> CXXAudioToolbox::CAAudioFile::WritePackets(bool inUseCache, UInt32 inNumBytes, const AudioStreamPacketDescription * _Nullable inPacketDescriptions, SInt64 inStartingPacket, UInt32& ioNumPackets, const void *inBuffer) noexcept
{
	const auto result = AudioFileWritePackets(audioFile_, inUseCache, inNumBytes, inPacketDescriptions, inStartingPacket, &ioNumPackets, inBuffer);
	if(result != noErr)
		return std::unexpected(result);
	return {};
}

std::expected<UInt32, OSStatus> CXXAudioToolbox::CAAudioFile::GetUserDataSize(UInt32 inUserDataID, UInt32 inIndex) noexcept
{
	UInt32 size;
	const auto result = AudioFileGetUserDataSize(audioFile_, inUserDataID, inIndex, &size);
	if(result != noErr)
		return std::unexpected(result);
	return size;
}

std::expected<void, OSStatus> CXXAudioToolbox::CAAudioFile::GetUserData(UInt32 inUserDataID, UInt32 inIndex, UInt32& ioUserDataSize, void *outUserData) const noexcept
{
	const auto result = AudioFileGetUserData(audioFile_, inUserDataID, inIndex, &ioUserDataSize, outUserData);
	if(result != noErr)
		return std::unexpected(result);
	return {};
}

std::expected<void, OSStatus> CXXAudioToolbox::CAAudioFile::SetUserData(UInt32 inUserDataID, UInt32 inIndex, UInt32 inUserDataSize, const void *inUserData) noexcept
{
	const auto result = AudioFileSetUserData(audioFile_, inUserDataID, inIndex, inUserDataSize, inUserData);
	if(result != noErr)
		return std::unexpected(result);
	return {};
}

std::expected<void, OSStatus> CXXAudioToolbox::CAAudioFile::RemoveUserData(UInt32 inUserDataID, UInt32 inIndex) noexcept
{
	const auto result = AudioFileRemoveUserData(audioFile_, inUserDataID, inIndex);
	if(result != noErr)
		return std::unexpected(result);
	return {};
}

std::expected<void, OSStatus> CXXAudioToolbox::CAAudioFile::GetPropertyInfo(AudioFilePropertyID inPropertyID, UInt32 * _Nullable outDataSize, UInt32 * _Nullable isWritable) const
{
	const auto result = AudioFileGetPropertyInfo(audioFile_, inPropertyID, outDataSize, isWritable);
	if(result != noErr)
		return std::unexpected(result);
	return {};
}

std::expected<void, OSStatus> CXXAudioToolbox::CAAudioFile::GetProperty(AudioFilePropertyID inPropertyID, UInt32& ioDataSize, void *outPropertyData) const noexcept
{
	const auto result = AudioFileGetProperty(audioFile_, inPropertyID, &ioDataSize, outPropertyData);
	if(result != noErr)
		return std::unexpected(result);
	return {};
}

std::expected<void, OSStatus> CXXAudioToolbox::CAAudioFile::SetProperty(AudioFilePropertyID inPropertyID, UInt32 inDataSize, const void *inPropertyData) noexcept
{
	const auto result = AudioFileSetProperty(audioFile_, inPropertyID, inDataSize, inPropertyData);
	if(result != noErr)
		return std::unexpected(result);
	return {};
}

std::expected<AudioFileTypeID, OSStatus> CXXAudioToolbox::CAAudioFile::FileFormat() const noexcept
{
	AudioFileTypeID fileFormat;
	UInt32 size = sizeof fileFormat;
	const auto result = AudioFileGetProperty(audioFile_, kAudioFilePropertyFileFormat, &size, &fileFormat);
	if(result != noErr)
		return std::unexpected(result);
	return fileFormat;
}

std::expected<CXXCoreAudio::CAStreamDescription, OSStatus> CXXAudioToolbox::CAAudioFile::DataFormat() const noexcept
{
	CXXCoreAudio::CAStreamDescription fileDataFormat;
	UInt32 size = sizeof fileDataFormat;
	const auto result = AudioFileGetProperty(audioFile_, kAudioFilePropertyDataFormat, &size, &fileDataFormat);
	if(result != noErr)
		return std::unexpected(result);
	return fileDataFormat;
}

// MARK: Global Properties

std::expected<UInt32, OSStatus> CXXAudioToolbox::CAAudioFile::GetGlobalInfoSize(AudioFilePropertyID inPropertyID, UInt32 inSpecifierSize, void * _Nullable inSpecifier) noexcept
{
	UInt32 size;
	const auto result = AudioFileGetGlobalInfoSize(inPropertyID, inSpecifierSize, inSpecifier, &size);
	if(result != noErr)
		return std::unexpected(result);
	return size;
}

std::expected<void, OSStatus> CXXAudioToolbox::CAAudioFile::GetGlobalInfo(AudioFilePropertyID inPropertyID, UInt32 inSpecifierSize, void * _Nullable inSpecifier, UInt32& ioDataSize, void *outPropertyData) noexcept
{
	const auto result = AudioFileGetGlobalInfo(inPropertyID, inSpecifierSize, inSpecifier, &ioDataSize, outPropertyData);
	if(result != noErr)
		return std::unexpected(result);
	return {};
}

std::expected<std::vector<AudioFileTypeID>, OSStatus> CXXAudioToolbox::CAAudioFile::ReadableTypes() noexcept
{
	UInt32 size;
	auto result = AudioFileGetGlobalInfoSize(kAudioFileGlobalInfo_ReadableTypes, 0, nullptr, &size);
	if(result != noErr)
		return std::unexpected(result);

	try {
		const auto count = size / sizeof(AudioFileTypeID);
		auto types = std::vector<AudioFileTypeID>(count);
		result = AudioFileGetGlobalInfo(kAudioFileGlobalInfo_ReadableTypes, 0, nullptr, &size, types.data());
		if(result != noErr)
			return std::unexpected(result);
		types.resize(size / sizeof(AudioFileTypeID));
		return types;
	} catch(const std::exception& e) {
		return std::unexpected(kAudio_MemFullError);
	}
}

std::expected<std::vector<AudioFileTypeID>, OSStatus> CXXAudioToolbox::CAAudioFile::WritableTypes() noexcept
{
	UInt32 size;
	auto result = AudioFileGetGlobalInfoSize(kAudioFileGlobalInfo_WritableTypes, 0, nullptr, &size);
	if(result != noErr)
		return std::unexpected(result);

	try {
		const auto count = size / sizeof(AudioFileTypeID);
		auto types = std::vector<AudioFileTypeID>(count);
		result = AudioFileGetGlobalInfo(kAudioFileGlobalInfo_WritableTypes, 0, nullptr, &size, types.data());
		if(result != noErr)
			return std::unexpected(result);
		types.resize(size / sizeof(AudioFileTypeID));
		return types;
	} catch(const std::exception& e) {
		return std::unexpected(kAudio_MemFullError);
	}
}

std::expected<CXXCFRef::CFString, OSStatus> CXXAudioToolbox::CAAudioFile::CopyFileTypeName(AudioFileTypeID type) noexcept
{
	CFStringRef fileTypeName;
	UInt32 size = sizeof fileTypeName;
	const auto result = AudioFileGetGlobalInfo(kAudioFileGlobalInfo_FileTypeName, sizeof type, &type, &size, &fileTypeName);
	if(result != noErr)
		return std::unexpected(result);
	return CXXCFRef::CFString{fileTypeName};
}

std::expected<std::vector<CXXCoreAudio::CAStreamDescription>, OSStatus> CXXAudioToolbox::CAAudioFile::AvailableStreamDescriptions(AudioFileTypeID fileType, AudioFormatID formatID) noexcept
{
	AudioFileTypeAndFormatID spec{ fileType, formatID };
	UInt32 size;
	auto result = AudioFileGetGlobalInfoSize(kAudioFileGlobalInfo_AvailableStreamDescriptionsForFormat, sizeof spec, &spec, &size);
	if(result != noErr)
		return std::unexpected(result);

	try {
		static_assert(sizeof(AudioStreamBasicDescription) == sizeof(CXXCoreAudio::CAStreamDescription), "AudioStreamBasicDescription size mismatch");
		const auto count = size / sizeof(AudioStreamBasicDescription);
		auto streamDescriptions = std::vector<CXXCoreAudio::CAStreamDescription>(count);
		result = AudioFileGetGlobalInfo(kAudioFileGlobalInfo_AvailableStreamDescriptionsForFormat, sizeof spec, &spec, &size, streamDescriptions.data());
		if(result != noErr)
			return std::unexpected(result);
		streamDescriptions.resize(size / sizeof(AudioStreamBasicDescription));
		return streamDescriptions;
	} catch(const std::exception& e) {
		return std::unexpected(kAudio_MemFullError);
	}
}

std::expected<std::vector<AudioFormatID>, OSStatus> CXXAudioToolbox::CAAudioFile::AvailableFormatIDs(AudioFileTypeID type) noexcept
{
	UInt32 size;
	auto result = AudioFileGetGlobalInfoSize(kAudioFileGlobalInfo_AvailableFormatIDs, sizeof type, &type, &size);
	if(result != noErr)
		return std::unexpected(result);

	try {
		const auto count = size / sizeof(AudioFormatID);
		auto formatIDs = std::vector<AudioFormatID>(count);
		result = AudioFileGetGlobalInfo(kAudioFileGlobalInfo_AvailableFormatIDs, sizeof type, &type, &size, formatIDs.data());
		if(result != noErr)
			return std::unexpected(result);
		formatIDs.resize(size / sizeof(AudioFormatID));
		return formatIDs;
	} catch(const std::exception& e) {
		return std::unexpected(kAudio_MemFullError);
	}
}

std::expected<CXXCFRef::CFArray, OSStatus> CXXAudioToolbox::CAAudioFile::CopyAllExtensions() noexcept
{
	CFArrayRef extensions;
	UInt32 size = sizeof extensions;
	const auto result = AudioFileGetGlobalInfo(kAudioFileGlobalInfo_AllExtensions, 0, nullptr, &size, &extensions);
	if(result != noErr)
		return std::unexpected(result);
	return CXXCFRef::CFArray{extensions};
}

std::expected<CXXCFRef::CFArray, OSStatus> CXXAudioToolbox::CAAudioFile::CopyAllUTIs() noexcept
{
	CFArrayRef utis;
	UInt32 size = sizeof utis;
	const auto result = AudioFileGetGlobalInfo(kAudioFileGlobalInfo_AllUTIs, 0, nullptr, &size, &utis);
	if(result != noErr)
		return std::unexpected(result);
	return CXXCFRef::CFArray{utis};
}

std::expected<CXXCFRef::CFArray, OSStatus> CXXAudioToolbox::CAAudioFile::CopyAllMIMETypes() noexcept
{
	CFArrayRef mimeTypes;
	UInt32 size = sizeof mimeTypes;
	const auto result = AudioFileGetGlobalInfo(kAudioFileGlobalInfo_AllMIMETypes, 0, nullptr, &size, &mimeTypes);
	if(result != noErr)
		return std::unexpected(result);
	return CXXCFRef::CFArray{mimeTypes};
}

std::expected<CXXCFRef::CFArray, OSStatus> CXXAudioToolbox::CAAudioFile::CopyExtensionsForType(AudioFileTypeID type) noexcept
{
	CFArrayRef extensions;
	UInt32 size = sizeof extensions;
	const auto result = AudioFileGetGlobalInfo(kAudioFileGlobalInfo_ExtensionsForType, 0, nullptr, &size, &extensions);
	if(result != noErr)
		return std::unexpected(result);
	return CXXCFRef::CFArray{extensions};
}

std::expected<CXXCFRef::CFArray, OSStatus> CXXAudioToolbox::CAAudioFile::CopyUTIsForType(AudioFileTypeID type) noexcept
{
	CFArrayRef utis;
	UInt32 size = sizeof utis;
	const auto result = AudioFileGetGlobalInfo(kAudioFileGlobalInfo_UTIsForType, 0, nullptr, &size, &utis);
	if(result != noErr)
		return std::unexpected(result);
	return CXXCFRef::CFArray{utis};
}

std::expected<CXXCFRef::CFArray, OSStatus> CXXAudioToolbox::CAAudioFile::CopyMIMETypesForType(AudioFileTypeID type) noexcept
{
	CFArrayRef mimeTypes;
	UInt32 size = sizeof mimeTypes;
	const auto result = AudioFileGetGlobalInfo(kAudioFileGlobalInfo_MIMETypesForType, 0, nullptr, &size, &mimeTypes);
	if(result != noErr)
		return std::unexpected(result);
	return CXXCFRef::CFArray{mimeTypes};
}

std::expected<std::vector<AudioFileTypeID>, OSStatus> CXXAudioToolbox::CAAudioFile::TypesForMIMEType(CFStringRef mimeType) noexcept
{
	UInt32 size;
	auto result = AudioFileGetGlobalInfoSize(kAudioFileGlobalInfo_TypesForMIMEType, sizeof mimeType, const_cast<void *>(reinterpret_cast<const void *>(mimeType)), &size);
	if(result != noErr)
		return std::unexpected(result);

	try {
		const auto count = size / sizeof(AudioFileTypeID);
		auto types = std::vector<AudioFileTypeID>(count);
		result = AudioFileGetGlobalInfo(kAudioFileGlobalInfo_TypesForMIMEType, sizeof mimeType, const_cast<void *>(reinterpret_cast<const void *>(mimeType)), &size, types.data());
		if(result != noErr)
			return std::unexpected(result);
		types.resize(size / sizeof(AudioFileTypeID));
		return types;
	} catch(const std::exception& e) {
		return std::unexpected(kAudio_MemFullError);
	}
}

std::expected<std::vector<AudioFileTypeID>, OSStatus> CXXAudioToolbox::CAAudioFile::TypesForUTI(CFStringRef uti) noexcept
{
	UInt32 size;
	auto result = AudioFileGetGlobalInfoSize(kAudioFileGlobalInfo_TypesForUTI, sizeof uti, const_cast<void *>(reinterpret_cast<const void *>(uti)), &size);
	if(result != noErr)
		return std::unexpected(result);

	try {
		const auto count = size / sizeof(AudioFileTypeID);
		auto types = std::vector<AudioFileTypeID>(count);
		result = AudioFileGetGlobalInfo(kAudioFileGlobalInfo_TypesForUTI, sizeof uti, const_cast<void *>(reinterpret_cast<const void *>(uti)), &size, types.data());
		if(result != noErr)
			return std::unexpected(result);
		types.resize(size / sizeof(AudioFileTypeID));
		return types;
	} catch(const std::exception& e) {
		return std::unexpected(kAudio_MemFullError);
	}
}

std::expected<std::vector<AudioFileTypeID>, OSStatus> CXXAudioToolbox::CAAudioFile::TypesForExtension(CFStringRef extension) noexcept
{
	UInt32 size;
	auto result = AudioFileGetGlobalInfoSize(kAudioFileGlobalInfo_TypesForExtension, sizeof extension, const_cast<void *>(reinterpret_cast<const void *>(extension)), &size);
	if(result != noErr)
		return std::unexpected(result);

	try {
		const auto count = size / sizeof(AudioFileTypeID);
		auto types = std::vector<AudioFileTypeID>(count);
		result = AudioFileGetGlobalInfo(kAudioFileGlobalInfo_TypesForExtension, sizeof extension, const_cast<void *>(reinterpret_cast<const void *>(extension)), &size, types.data());
		if(result != noErr)
			return std::unexpected(result);
		types.resize(size / sizeof(AudioFileTypeID));
		return types;
	} catch(const std::exception& e) {
		return std::unexpected(kAudio_MemFullError);
	}
}
