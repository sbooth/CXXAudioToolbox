//
// Copyright © 2021-2025 Stephen F. Booth
// Part of https://github.com/sbooth/CXXAudioToolbox
// MIT license
//

#import <exception>
#import <system_error>
#import <utility>

#import "ATAudioFile.hpp"

namespace {

/// A std::error_category for return values from the AudioFile API.
class AudioFileErrorCategory : public std::error_category {
public:
	virtual const char * name() const noexcept override final { return "AudioFile"; }
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
				// AudioFile.h
			case kAudioFileUnspecifiedError: 				return "An unspecified error has occurred";
			case kAudioFileUnsupportedFileTypeError: 		return "The file type is not supported";
			case kAudioFileUnsupportedDataFormatError: 		return "The data format is not supported by this file type";
			case kAudioFileUnsupportedPropertyError: 		return "The property is not supported";
			case kAudioFileBadPropertySizeError: 			return "The size of the property data was not correct";
			case kAudioFilePermissionsError: 				return "The operation violated the file permissions";
			case kAudioFileNotOptimizedError: 				return "There are chunks following the audio data chunk that prevent extending the audio data chunk. The file must be optimized in order to write more audio data.";
			case kAudioFileInvalidChunkError: 				return "The chunk does not exist in the file or is not supported by the file";
			case kAudioFileDoesNotAllow64BitDataSizeError: 	return "The a file offset was too large for the file type. AIFF and WAVE have a 32 bit file size limit.";
			case kAudioFileInvalidPacketOffsetError: 		return "A packet offset was past the end of the file, or not at the end of the file when writing a VBR format, or a corrupt packet size was read when building the packet table.";
			case kAudioFileInvalidPacketDependencyError: 		return "Either the packet dependency info that's necessary for the audio format has not been provided, or the provided packet dependency info indicates dependency on a packet that's unavailable.";
			case kAudioFileInvalidFileError:				return "The file is malformed, or otherwise not a valid instance of an audio file of its type";
			case kAudioFileOperationNotSupportedError: 		return "The operation cannot be performed";
			case kAudioFileNotOpenError:					return "The file is closed";
			case kAudioFileEndOfFileError: 					return "End of file";
			case kAudioFilePositionError: 					return "Invalid file position";
//			case kAudioFileFileNotFoundError: 				return "File not found";
			default:										return "Unknown AudioFile error";
		}
	}
};

/// Global instance of AudioFile error category.
const AudioFileErrorCategory audioFileErrorCategory_;

/// Throws a std::system_error in the AudioFileErrorCategory if result != kAudio_NoError.
/// @param result An OSStatus result code.
/// @param operation An optional string describing the operation that produced result.
/// @throw std::system_error in the AudioFileErrorCategory.
inline void ThrowIfAudioFileError(OSStatus result, const char * const operation = nullptr)
{
	if(__builtin_expect(result != kAudio_NoError, false))
		throw std::system_error(result, audioFileErrorCategory_, operation);
}

} /* namespace */

CXXAudioToolbox::ATAudioFile::ATAudioFile(ATAudioFile&& rhs) noexcept
: mAudioFileID{std::exchange(rhs.mAudioFileID, nullptr)}
{}

CXXAudioToolbox::ATAudioFile& CXXAudioToolbox::ATAudioFile::operator=(ATAudioFile&& rhs) noexcept
{
	if(this != &rhs) {
		if(mAudioFileID)
			AudioFileClose(mAudioFileID);
		mAudioFileID = std::exchange(rhs.mAudioFileID, nullptr);
	}
	return *this;
}

/// Destroys the audio file and releases all associated resources.
CXXAudioToolbox::ATAudioFile::~ATAudioFile() noexcept
{
	if(mAudioFileID)
		AudioFileClose(mAudioFileID);
}

void CXXAudioToolbox::ATAudioFile::OpenURL(CFURLRef inURL, AudioFilePermissions inPermissions, AudioFileTypeID inFileTypeHint)
{
	Close();
	auto result = AudioFileOpenURL(inURL, inPermissions, inFileTypeHint, &mAudioFileID);
	ThrowIfAudioFileError(result, "AudioFileOpenURL");
}

void CXXAudioToolbox::ATAudioFile::CreateWithURL(CFURLRef inURL, AudioFileTypeID inFileType, const AudioStreamBasicDescription& inFormat, AudioFileFlags inFlags)
{
	Close();
	auto result = AudioFileCreateWithURL(inURL, inFileType, &inFormat, inFlags, &mAudioFileID);
	ThrowIfAudioFileError(result, "AudioFileCreateWithURL");
}

void CXXAudioToolbox::ATAudioFile::InitializeWithCallbacks(void *inClientData, AudioFile_ReadProc inReadFunc, AudioFile_WriteProc inWriteFunc, AudioFile_GetSizeProc inGetSizeFunc, AudioFile_SetSizeProc inSetSizeFunc, AudioFileTypeID inFileType, const AudioStreamBasicDescription& inFormat, AudioFileFlags inFlags)
{
	Close();
	auto result = AudioFileInitializeWithCallbacks(inClientData, inReadFunc, inWriteFunc, inGetSizeFunc, inSetSizeFunc, inFileType, &inFormat, inFlags, &mAudioFileID);
	ThrowIfAudioFileError(result, "AudioFileInitializeWithCallbacks");
}

void CXXAudioToolbox::ATAudioFile::OpenWithCallbacks(void *inClientData, AudioFile_ReadProc inReadFunc, AudioFile_WriteProc _Nullable inWriteFunc, AudioFile_GetSizeProc inGetSizeFunc, AudioFile_SetSizeProc _Nullable inSetSizeFunc, AudioFileTypeID inFileTypeHint)
{
	Close();
	auto result = AudioFileOpenWithCallbacks(inClientData, inReadFunc, inWriteFunc, inGetSizeFunc, inSetSizeFunc, inFileTypeHint, &mAudioFileID);
	ThrowIfAudioFileError(result, "AudioFileOpenWithCallbacks");
}

void CXXAudioToolbox::ATAudioFile::Close()
{
	if(mAudioFileID) {
		auto result = AudioFileClose(mAudioFileID);
		mAudioFileID = nullptr;
		ThrowIfAudioFileError(result, "AudioFileClose");
	}
}

void CXXAudioToolbox::ATAudioFile::Optimize()
{
	auto result = AudioFileOptimize(mAudioFileID);
	ThrowIfAudioFileError(result, "AudioFileOptimize");
}

OSStatus CXXAudioToolbox::ATAudioFile::ReadBytes(bool inUseCache, SInt64 inStartingByte, UInt32& ioNumBytes, void *outBuffer)
{
	auto result = AudioFileReadBytes(mAudioFileID, inUseCache, inStartingByte, &ioNumBytes, outBuffer);
	switch(result) {
		case noErr:
		case kAudioFileEndOfFileError:
			break;
		default:
			ThrowIfAudioFileError(result, "AudioFileReadBytes");
			break;
	}
	return result;
}

void CXXAudioToolbox::ATAudioFile::WriteBytes(bool inUseCache, SInt64 inStartingByte, UInt32& ioNumBytes, const void *inBuffer)
{
	auto result = AudioFileWriteBytes(mAudioFileID, inUseCache, inStartingByte, &ioNumBytes, inBuffer);
	ThrowIfAudioFileError(result, "AudioFileWriteBytes");
}

OSStatus CXXAudioToolbox::ATAudioFile::ReadPacketData(bool inUseCache, UInt32& ioNumBytes, AudioStreamPacketDescription * _Nullable outPacketDescriptions, SInt64 inStartingPacket, UInt32& ioNumPackets, void * _Nullable outBuffer)
{
	auto result = AudioFileReadPacketData(mAudioFileID, inUseCache, &ioNumBytes, outPacketDescriptions, inStartingPacket, &ioNumPackets, outBuffer);
	switch(result) {
		case noErr:
		case kAudioFileEndOfFileError:
			break;
		default:
			ThrowIfAudioFileError(result, "AudioFileReadPacketData");
			break;
	}
	return result;
}

void CXXAudioToolbox::ATAudioFile::WritePackets(bool inUseCache, UInt32 inNumBytes, const AudioStreamPacketDescription * _Nullable inPacketDescriptions, SInt64 inStartingPacket, UInt32& ioNumPackets, const void *inBuffer)
{
	auto result = AudioFileWritePackets(mAudioFileID, inUseCache, inNumBytes, inPacketDescriptions, inStartingPacket, &ioNumPackets, inBuffer);
	ThrowIfAudioFileError(result, "AudioFileWritePackets");
}

UInt32 CXXAudioToolbox::ATAudioFile::GetUserDataSize(UInt32 inUserDataID, UInt32 inIndex)
{
	UInt32 size;
	auto result = AudioFileGetUserDataSize(mAudioFileID, inUserDataID, inIndex, &size);
	ThrowIfAudioFileError(result, "AudioFileGetUserDataSize");
	return size;
}

void CXXAudioToolbox::ATAudioFile::GetUserData(UInt32 inUserDataID, UInt32 inIndex, UInt32& ioUserDataSize, void *outUserData) const
{
	auto result = AudioFileGetUserData(mAudioFileID, inUserDataID, inIndex, &ioUserDataSize, outUserData);
	ThrowIfAudioFileError(result, "AudioFileGetUserData");
}

void CXXAudioToolbox::ATAudioFile::SetUserData(UInt32 inUserDataID, UInt32 inIndex, UInt32 inUserDataSize, const void *inUserData)
{
	auto result = AudioFileSetUserData(mAudioFileID, inUserDataID, inIndex, inUserDataSize, inUserData);
	ThrowIfAudioFileError(result, "AudioFileGetUserData");
}

void CXXAudioToolbox::ATAudioFile::RemoveUserData(UInt32 inUserDataID, UInt32 inIndex)
{
	auto result = AudioFileRemoveUserData(mAudioFileID, inUserDataID, inIndex);
	ThrowIfAudioFileError(result, "AudioFileRemoveUserData");
}

UInt32 CXXAudioToolbox::ATAudioFile::GetPropertyInfo(AudioFilePropertyID inPropertyID, UInt32 * _Nullable isWritable) const
{
	UInt32 size;
	auto result = AudioFileGetPropertyInfo(mAudioFileID, inPropertyID, &size, isWritable);
	ThrowIfAudioFileError(result, "AudioFileGetPropertyInfo");
	return size;
}

void CXXAudioToolbox::ATAudioFile::GetProperty(AudioFilePropertyID inPropertyID, UInt32& ioDataSize, void *outPropertyData) const
{
	auto result = AudioFileGetProperty(mAudioFileID, inPropertyID, &ioDataSize, outPropertyData);
	ThrowIfAudioFileError(result, "AudioFileGetProperty");
}

void CXXAudioToolbox::ATAudioFile::SetProperty(AudioFilePropertyID inPropertyID, UInt32 inDataSize, const void *inPropertyData)
{
	auto result = AudioFileSetProperty(mAudioFileID, inPropertyID, inDataSize, inPropertyData);
	ThrowIfAudioFileError(result, "AudioFileSetProperty");
}

AudioFileTypeID CXXAudioToolbox::ATAudioFile::FileFormat() const
{
	AudioFileTypeID fileFormat;
	UInt32 size = sizeof(fileFormat);
	GetProperty(kAudioFilePropertyFileFormat, size, &fileFormat);
	return fileFormat;
}

CXXCoreAudio::CAStreamDescription CXXAudioToolbox::ATAudioFile::DataFormat() const
{
	CXXCoreAudio::CAStreamDescription fileDataFormat;
	UInt32 size = sizeof(fileDataFormat);
	GetProperty(kAudioFilePropertyDataFormat, size, &fileDataFormat);
	return fileDataFormat;
}

// MARK: Global Properties

UInt32 CXXAudioToolbox::ATAudioFile::GetGlobalInfoSize(AudioFilePropertyID inPropertyID, UInt32 inSpecifierSize, void * _Nullable inSpecifier)
{
	UInt32 size;
	auto result = AudioFileGetGlobalInfoSize(inPropertyID, inSpecifierSize, inSpecifier, &size);
	ThrowIfAudioFileError(result, "AudioFileGetGlobalInfoSize");
	return size;
}

void CXXAudioToolbox::ATAudioFile::GetGlobalInfo(AudioFilePropertyID inPropertyID, UInt32 inSpecifierSize, void * _Nullable inSpecifier, UInt32& ioDataSize, void *outPropertyData)
{
	auto result = AudioFileGetGlobalInfo(inPropertyID, inSpecifierSize, inSpecifier, &ioDataSize, outPropertyData);
	ThrowIfAudioFileError(result, "AudioFileGetGlobalInfo");
}

std::vector<AudioFileTypeID> CXXAudioToolbox::ATAudioFile::ReadableTypes()
{
	auto size = GetGlobalInfoSize(kAudioFileGlobalInfo_ReadableTypes, 0, nullptr);
	auto count = size / sizeof(AudioFileTypeID);
	auto types = std::vector<AudioFileTypeID>(count);
	GetGlobalInfo(kAudioFileGlobalInfo_ReadableTypes, 0, nullptr, size, &types[0]);
	return types;
}

std::vector<AudioFileTypeID> CXXAudioToolbox::ATAudioFile::WritableTypes()
{
	auto size = GetGlobalInfoSize(kAudioFileGlobalInfo_WritableTypes, 0, nullptr);
	auto count = size / sizeof(AudioFileTypeID);
	auto types = std::vector<AudioFileTypeID>(count);
	GetGlobalInfo(kAudioFileGlobalInfo_WritableTypes, 0, nullptr, size, &types[0]);
	return types;
}

CFStringRef CXXAudioToolbox::ATAudioFile::CopyFileTypeName(AudioFileTypeID type)
{
	CFStringRef s;
	UInt32 size = sizeof(s);
	GetGlobalInfo(kAudioFileGlobalInfo_FileTypeName, sizeof(type), &type, size, &s);
	return s;
}

std::vector<CXXCoreAudio::CAStreamDescription> CXXAudioToolbox::ATAudioFile::AvailableStreamDescriptions(AudioFileTypeID fileType, AudioFormatID formatID)
{
	AudioFileTypeAndFormatID spec{ fileType, formatID };
	auto size = GetGlobalInfoSize(kAudioFileGlobalInfo_AvailableStreamDescriptionsForFormat, sizeof(spec), &spec);
	auto count = size / sizeof(AudioStreamBasicDescription);
	auto streamDescriptions = std::vector<CXXCoreAudio::CAStreamDescription>(count);
	GetGlobalInfo(kAudioFileGlobalInfo_AvailableStreamDescriptionsForFormat, sizeof(spec), &spec, size, &streamDescriptions[0]);
	return streamDescriptions;
}

std::vector<AudioFormatID> CXXAudioToolbox::ATAudioFile::AvailableFormatIDs(AudioFileTypeID type)
{
	auto size = GetGlobalInfoSize(kAudioFileGlobalInfo_AvailableFormatIDs, sizeof(type), &type);
	auto count = size / sizeof(AudioFormatID);
	auto formatIDs = std::vector<AudioFormatID>(count);
	GetGlobalInfo(kAudioFileGlobalInfo_AvailableFormatIDs, sizeof(type), &type, size, &formatIDs[0]);
	return formatIDs;
}


CFArrayRef CXXAudioToolbox::ATAudioFile::CopyAllExtensions()
{
	CFArrayRef a;
	UInt32 size = sizeof(a);
	GetGlobalInfo(kAudioFileGlobalInfo_AllExtensions, 0, nullptr, size, &a);
	return a;
}

CFArrayRef CXXAudioToolbox::ATAudioFile::CopyAllUTIs()
{
	CFArrayRef a;
	UInt32 size = sizeof(a);
	GetGlobalInfo(kAudioFileGlobalInfo_AllUTIs, 0, nullptr, size, &a);
	return a;
}

CFArrayRef CXXAudioToolbox::ATAudioFile::CopyAllMIMETypes()
{
	CFArrayRef a;
	UInt32 size = sizeof(a);
	GetGlobalInfo(kAudioFileGlobalInfo_AllMIMETypes, 0, nullptr, size, &a);
	return a;
}


CFArrayRef CXXAudioToolbox::ATAudioFile::CopyExtensionsForType(AudioFileTypeID type)
{
	CFArrayRef a;
	UInt32 size = sizeof(a);
	GetGlobalInfo(kAudioFileGlobalInfo_ExtensionsForType, sizeof(type), &type, size, &a);
	return a;
}

CFArrayRef CXXAudioToolbox::ATAudioFile::CopyUTIsForType(AudioFileTypeID type)
{
	CFArrayRef a;
	UInt32 size = sizeof(a);
	GetGlobalInfo(kAudioFileGlobalInfo_UTIsForType, sizeof(type), &type, size, &a);
	return a;
}

CFArrayRef CXXAudioToolbox::ATAudioFile::CopyMIMETypesForType(AudioFileTypeID type)
{
	CFArrayRef mimeTypes;
	UInt32 size = sizeof(mimeTypes);
	GetGlobalInfo(kAudioFileGlobalInfo_MIMETypesForType, sizeof(type), &type, size, &mimeTypes);
	return mimeTypes;
}


std::vector<AudioFileTypeID> CXXAudioToolbox::ATAudioFile::TypesForMIMEType(CFStringRef mimeType)
{
	auto size = GetGlobalInfoSize(kAudioFileGlobalInfo_TypesForMIMEType, sizeof(mimeType), const_cast<void *>(reinterpret_cast<const void *>(mimeType)));
	auto count = size / sizeof(AudioFileTypeID);
	auto types = std::vector<AudioFileTypeID>(count);
	GetGlobalInfo(kAudioFileGlobalInfo_TypesForMIMEType, sizeof(mimeType), const_cast<void *>(reinterpret_cast<const void *>(mimeType)), size, &types[0]);
	return types;
}

std::vector<AudioFileTypeID> CXXAudioToolbox::ATAudioFile::TypesForUTI(CFStringRef uti)
{
	auto size = GetGlobalInfoSize(kAudioFileGlobalInfo_TypesForUTI, sizeof(uti), const_cast<void *>(reinterpret_cast<const void *>(uti)));
	auto count = size / sizeof(AudioFileTypeID);
	auto types = std::vector<AudioFileTypeID>(count);
	GetGlobalInfo(kAudioFileGlobalInfo_TypesForUTI, sizeof(uti), const_cast<void *>(reinterpret_cast<const void *>(uti)), size, &types[0]);
	return types;
}

std::vector<AudioFileTypeID> CXXAudioToolbox::ATAudioFile::TypesForExtension(CFStringRef extension)
{
	auto size = GetGlobalInfoSize(kAudioFileGlobalInfo_TypesForExtension, sizeof(extension), const_cast<void *>(reinterpret_cast<const void *>(extension)));
	auto count = size / sizeof(AudioFileTypeID);
	auto types = std::vector<AudioFileTypeID>(count);
	GetGlobalInfo(kAudioFileGlobalInfo_TypesForExtension, sizeof(extension), const_cast<void *>(reinterpret_cast<const void *>(extension)), size, &types[0]);
	return types;
}
