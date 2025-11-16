//
// Copyright © 2021-2025 Stephen F. Booth
// Part of https://github.com/sbooth/CXXAudioToolbox
// MIT license
//

#pragma once

#import <vector>

#import <AudioToolbox/AudioFile.h>

#import <CAStreamDescription.hpp>

CF_ASSUME_NONNULL_BEGIN

namespace CXXAudioToolbox {

/// An AudioFile wrapper.
class ATAudioFile final {
public:

	/// Creates an audio file.
	ATAudioFile() noexcept = default;

	// This class is non-copyable
	ATAudioFile(const ATAudioFile&) = delete;

	// This class is non-assignable
	ATAudioFile& operator=(const ATAudioFile&) = delete;

	/// Move constructor.
	ATAudioFile(ATAudioFile&& rhs) noexcept;

	/// Move assignment operator.
	ATAudioFile& operator=(ATAudioFile&& rhs) noexcept;

	/// Destroys the audio file and releases all associated resources.
	~ATAudioFile() noexcept;

	/// Returns true if this object's internal AudioFile object is not null.
	explicit operator bool() const noexcept
	{
		return mAudioFileID != nullptr;
	}

	/// Returns true if this object's internal AudioFile object is not null.
	bool IsValid() const noexcept
	{
		return mAudioFileID != nullptr;
	}

	/// Returns the object's internal AudioFile object.
	operator AudioFileID const _Nullable () const noexcept
	{
		return mAudioFileID;
	}

	/// Opens an existing audio file.
	/// @throw std::system_error.
	void OpenURL(CFURLRef inURL, AudioFilePermissions inPermissions, AudioFileTypeID inFileTypeHint);

	/// Creates a new audio file (or initialises an existing file).
	/// @throw std::system_error.
	void CreateWithURL(CFURLRef inURL, AudioFileTypeID inFileType, const AudioStreamBasicDescription& inFormat, AudioFileFlags inFlags);

	/// Wipes clean an existing file. You provide callbacks that the AudioFile API will use to get the data.
	/// @throw std::system_error.
	void InitializeWithCallbacks(void *inClientData, AudioFile_ReadProc inReadFunc, AudioFile_WriteProc inWriteFunc, AudioFile_GetSizeProc inGetSizeFunc, AudioFile_SetSizeProc inSetSizeFunc, AudioFileTypeID inFileType, const AudioStreamBasicDescription& inFormat, AudioFileFlags inFlags);

	/// Opens an existing file. You provide callbacks that the AudioFile API will use to get the data.
	/// @throw std::system_error.
	void OpenWithCallbacks(void *inClientData, AudioFile_ReadProc inReadFunc, AudioFile_WriteProc _Nullable inWriteFunc, AudioFile_GetSizeProc inGetSizeFunc, AudioFile_SetSizeProc _Nullable inSetSizeFunc, AudioFileTypeID inFileTypeHint);

	/// Close an existing audio file.
	/// @throw std::system_error.
	void Close();

	/// Moves the audio data to the end of the file and other internal optimizations of the file structure.
	/// @throw std::system_error.
	void Optimize();

	/// Reads bytes of audio data from the audio file.
	/// @throw std::system_error.
	OSStatus ReadBytes(bool inUseCache, SInt64 inStartingByte, UInt32& ioNumBytes, void *outBuffer);

	/// Writes bytes of audio data to the audio file.
	/// @throw std::system_error.
	void WriteBytes(bool inUseCache, SInt64 inStartingByte, UInt32& ioNumBytes, const void *inBuffer);

	/// Reads packets of audio data from the audio file.
	/// @throw std::system_error.
	OSStatus ReadPacketData(bool inUseCache, UInt32& ioNumBytes, AudioStreamPacketDescription * _Nullable outPacketDescriptions, SInt64 inStartingPacket, UInt32& ioNumPackets, void * _Nullable outBuffer);

	/// Writes packets of audio data to the audio file.
	/// @throw std::system_error.
	void WritePackets(bool inUseCache, UInt32 inNumBytes, const AudioStreamPacketDescription * _Nullable inPacketDescriptions, SInt64 inStartingPacket, UInt32& ioNumPackets, const void *inBuffer);

	/// Gets the size of user data in a file.
	/// @throw std::system_error.
	UInt32 GetUserDataSize(UInt32 inUserDataID, UInt32 inIndex);

	/// Gets the data of a chunk in a file.
	/// @throw std::system_error.
	void GetUserData(UInt32 inUserDataID, UInt32 inIndex, UInt32& ioUserDataSize, void *outUserData) const;

	/// Sets the data of a chunk in a file.
	/// @throw std::system_error.
	void SetUserData(UInt32 inUserDataID, UInt32 inIndex, UInt32 inUserDataSize, const void *inUserData);

	/// Removes a user chunk in a file.
	/// @throw std::system_error.
	void RemoveUserData(UInt32 inUserDataID, UInt32 inIndex);

	/// Gets information about the size of a property of an AudioFile and whether it can be set.
	/// @throw std::system_error.
	UInt32 GetPropertyInfo(AudioFilePropertyID inPropertyID, UInt32 * _Nullable isWritable) const;

	/// Copies the value for a property of an AudioFile into a buffer.
	/// @throw std::system_error.
	void GetProperty(AudioFilePropertyID inPropertyID, UInt32& ioDataSize, void *outPropertyData) const;

	/// Sets the value for a property of an AudioFile.
	/// @throw std::system_error.
	void SetProperty(AudioFilePropertyID inPropertyID, UInt32 inDataSize, const void *inPropertyData);

	/// Returns the file's format (kAudioFilePropertyFileFormat)
	/// @throw std::system_error.
	AudioFileTypeID FileFormat() const;

	/// Returns the file's data format (kAudioFilePropertyDataFormat)
	/// @throw std::system_error.
	CXXCoreAudio::CAStreamDescription DataFormat() const;

	// MARK: Global Properties

	/// Gets the size of a global audio file property.
	/// @throw std::system_error.
	static UInt32 GetGlobalInfoSize(AudioFilePropertyID inPropertyID, UInt32 inSpecifierSize, void * _Nullable inSpecifier);

	/// Copies the value of a global property into a buffer.
	/// @throw std::system_error.
	static void GetGlobalInfo(AudioFilePropertyID inPropertyID, UInt32 inSpecifierSize, void * _Nullable inSpecifier, UInt32& ioDataSize, void *outPropertyData);


	/// Returns an array of AudioFileTypeID containing the file types (i.e. AIFF, WAVE, etc) that can be opened for reading.
	/// @throw std::system_error.
	static std::vector<AudioFileTypeID> ReadableTypes();

	/// Returns an array of AudioFileTypeID containing the file types (i.e. AIFF, WAVE, etc) that can be opened for writing.
	/// @throw std::system_error.
	static std::vector<AudioFileTypeID> WritableTypes();

	/// Returns the name of type
	/// @note The caller is responsible for releasing the returned string.
	/// @throw std::system_error.
	static CFStringRef CopyFileTypeName(AudioFileTypeID type) CF_RETURNS_RETAINED;

	/// Returns an array of supported formats for the fileType and formatID combination
	/// @throw std::system_error.
	static std::vector<CXXCoreAudio::CAStreamDescription> AvailableStreamDescriptions(AudioFileTypeID fileType, AudioFormatID formatID);

	/// Returns an array of format IDs that can be read.
	/// @throw std::system_error.
	static std::vector<AudioFormatID> AvailableFormatIDs(AudioFileTypeID type);


	/// Returns an array of recognized file extensions
	/// @throw std::system_error.
	static CFArrayRef CopyAllExtensions() CF_RETURNS_RETAINED;

	/// Returns an array of recognized UTIs
	/// @throw std::system_error.
	static CFArrayRef CopyAllUTIs() CF_RETURNS_RETAINED;

	/// Returns an array of recognized MIME types
	/// @throw std::system_error.
	static CFArrayRef CopyAllMIMETypes() CF_RETURNS_RETAINED;


	/// Returns an array of file extensions for type
	/// @throw std::system_error.
	static CFArrayRef CopyExtensionsForType(AudioFileTypeID type) CF_RETURNS_RETAINED;

	/// Returns an array of UTIs for type
	/// @throw std::system_error.
	static CFArrayRef CopyUTIsForType(AudioFileTypeID type) CF_RETURNS_RETAINED;

	/// Returns an array of MIME types for type
	/// @throw std::system_error.
	static CFArrayRef CopyMIMETypesForType(AudioFileTypeID type) CF_RETURNS_RETAINED;


	/// Returns an array of AudioFileTypeID that support mimeType
	/// @throw std::system_error.
	static std::vector<AudioFileTypeID> TypesForMIMEType(CFStringRef mimeType);

	/// Returns an array of AudioFileTypeID that support uti
	/// @throw std::system_error.
	static std::vector<AudioFileTypeID> TypesForUTI(CFStringRef uti);

	/// Returns an array of AudioFileTypeID that support extension
	/// @throw std::system_error.
	static std::vector<AudioFileTypeID> TypesForExtension(CFStringRef extension);

private:
	/// The underlying AudioFile object.
	AudioFileID _Nullable mAudioFileID{nullptr};
};

} /* namespace CXXAudioToolbox */

CF_ASSUME_NONNULL_END
