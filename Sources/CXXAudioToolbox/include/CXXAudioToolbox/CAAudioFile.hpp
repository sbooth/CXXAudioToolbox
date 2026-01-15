//
// SPDX-FileCopyrightText: 2021 Stephen F. Booth <contact@sbooth.dev>
// SPDX-License-Identifier: MIT
//
// Part of https://github.com/sbooth/CXXAudioToolbox
//

#pragma once

#import <expected>
#import <utility>
#import <vector>

#import <AudioToolbox/AudioFile.h>

#import <CXXCFRef/CFRef.hpp>
#import <CXXCoreAudio/CAStreamDescription.hpp>

CF_ASSUME_NONNULL_BEGIN

namespace CXXAudioToolbox {

/// An AudioFile wrapper.
class CAAudioFile final {
public:
	/// Creates an audio file.
	CAAudioFile() noexcept = default;

	// This class is non-copyable
	CAAudioFile(const CAAudioFile&) = delete;

	// This class is non-assignable
	CAAudioFile& operator=(const CAAudioFile&) = delete;

	/// Move constructor.
	CAAudioFile(CAAudioFile&& other) noexcept;

	/// Move assignment operator.
	CAAudioFile& operator=(CAAudioFile&& other) noexcept;

	/// Destroys the audio file and releases all associated resources.
	~CAAudioFile() noexcept;


	/// Returns true if the managed AudioFile object is not null.
	[[nodiscard]] explicit operator bool() const noexcept;

	/// Returns the managed AudioFile object.
	[[nodiscard]] operator AudioFileID const _Nullable () const noexcept;

	
	/// Opens an existing audio file.
	/// @throw std::system_error.
	[[nodiscard]] std::expected<void, OSStatus> OpenURL(CFURLRef inURL, AudioFilePermissions inPermissions, AudioFileTypeID inFileTypeHint) noexcept;

	/// Creates a new audio file (or initialises an existing file).
	/// @throw std::system_error.
	[[nodiscard]] std::expected<void, OSStatus> CreateWithURL(CFURLRef inURL, AudioFileTypeID inFileType, const AudioStreamBasicDescription& inFormat, AudioFileFlags inFlags) noexcept;

	/// Wipes clean an existing file. You provide callbacks that the AudioFile API will use to get the data.
	/// @throw std::system_error.
	[[nodiscard]] std::expected<void, OSStatus> InitializeWithCallbacks(void *inClientData, AudioFile_ReadProc inReadFunc, AudioFile_WriteProc inWriteFunc, AudioFile_GetSizeProc inGetSizeFunc, AudioFile_SetSizeProc inSetSizeFunc, AudioFileTypeID inFileType, const AudioStreamBasicDescription& inFormat, AudioFileFlags inFlags) noexcept;

	/// Opens an existing file. You provide callbacks that the AudioFile API will use to get the data.
	/// @throw std::system_error.
	[[nodiscard]] std::expected<void, OSStatus> OpenWithCallbacks(void *inClientData, AudioFile_ReadProc inReadFunc, AudioFile_WriteProc _Nullable inWriteFunc, AudioFile_GetSizeProc inGetSizeFunc, AudioFile_SetSizeProc _Nullable inSetSizeFunc, AudioFileTypeID inFileTypeHint) noexcept;

	/// Close an existing audio file.
	/// @throw std::system_error.
	[[nodiscard]] std::expected<void, OSStatus> Close() noexcept;

	/// Moves the audio data to the end of the file and other internal optimizations of the file structure.
	/// @throw std::system_error.
	[[nodiscard]] std::expected<void, OSStatus> Optimize() noexcept;

	/// Reads bytes of audio data from the audio file.
	/// @throw std::system_error.
	[[nodiscard]] std::expected<OSStatus, OSStatus> ReadBytes(bool inUseCache, SInt64 inStartingByte, UInt32& ioNumBytes, void *outBuffer) noexcept;

	/// Writes bytes of audio data to the audio file.
	/// @throw std::system_error.
	[[nodiscard]] std::expected<void, OSStatus> WriteBytes(bool inUseCache, SInt64 inStartingByte, UInt32& ioNumBytes, const void *inBuffer) noexcept;

	/// Reads packets of audio data from the audio file.
	/// @throw std::system_error.
	[[nodiscard]] std::expected<OSStatus, OSStatus> ReadPacketData(bool inUseCache, UInt32& ioNumBytes, AudioStreamPacketDescription * _Nullable outPacketDescriptions, SInt64 inStartingPacket, UInt32& ioNumPackets, void * _Nullable outBuffer) noexcept;

	/// Writes packets of audio data to the audio file.
	/// @throw std::system_error.
	[[nodiscard]] std::expected<void, OSStatus> WritePackets(bool inUseCache, UInt32 inNumBytes, const AudioStreamPacketDescription * _Nullable inPacketDescriptions, SInt64 inStartingPacket, UInt32& ioNumPackets, const void *inBuffer) noexcept;

	/// Gets the size of user data in a file.
	/// @throw std::system_error.
	[[nodiscard]] std::expected<UInt32, OSStatus> GetUserDataSize(UInt32 inUserDataID, UInt32 inIndex) noexcept;

	/// Gets the data of a chunk in a file.
	/// @throw std::system_error.
	[[nodiscard]] std::expected<void, OSStatus> GetUserData(UInt32 inUserDataID, UInt32 inIndex, UInt32& ioUserDataSize, void *outUserData) const noexcept;

	/// Sets the data of a chunk in a file.
	/// @throw std::system_error.
	[[nodiscard]] std::expected<void, OSStatus> SetUserData(UInt32 inUserDataID, UInt32 inIndex, UInt32 inUserDataSize, const void *inUserData) noexcept;

	/// Removes a user chunk in a file.
	/// @throw std::system_error.
	[[nodiscard]] std::expected<void, OSStatus> RemoveUserData(UInt32 inUserDataID, UInt32 inIndex) noexcept;

	/// Gets information about the size of a property of an AudioFile and whether it can be set.
	/// @throw std::system_error.
	[[nodiscard]] std::expected<void, OSStatus> GetPropertyInfo(AudioFilePropertyID inPropertyID, UInt32 * _Nullable outDataSize, UInt32 * _Nullable isWritable) const;

	/// Copies the value for a property of an AudioFile into a buffer.
	/// @throw std::system_error.
	[[nodiscard]] std::expected<void, OSStatus> GetProperty(AudioFilePropertyID inPropertyID, UInt32& ioDataSize, void *outPropertyData) const noexcept;

	/// Sets the value for a property of an AudioFile.
	/// @throw std::system_error.
	[[nodiscard]] std::expected<void, OSStatus> SetProperty(AudioFilePropertyID inPropertyID, UInt32 inDataSize, const void *inPropertyData) noexcept;

	/// Returns the file's format (kAudioFilePropertyFileFormat)
	/// @throw std::system_error.
	[[nodiscard]] std::expected<AudioFileTypeID, OSStatus> FileFormat() const noexcept;

	/// Returns the file's data format (kAudioFilePropertyDataFormat)
	/// @throw std::system_error.
	[[nodiscard]] std::expected<CXXCoreAudio::CAStreamDescription, OSStatus> DataFormat() const noexcept;

	// MARK: Global Properties

	/// Gets the size of a global audio file property.
	/// @throw std::system_error.
	[[nodiscard]] static std::expected<UInt32, OSStatus> GetGlobalInfoSize(AudioFilePropertyID inPropertyID, UInt32 inSpecifierSize, void * _Nullable inSpecifier) noexcept;

	/// Copies the value of a global property into a buffer.
	/// @throw std::system_error.
	[[nodiscard]] static std::expected<void, OSStatus> GetGlobalInfo(AudioFilePropertyID inPropertyID, UInt32 inSpecifierSize, void * _Nullable inSpecifier, UInt32& ioDataSize, void *outPropertyData) noexcept;


	/// Returns an array of AudioFileTypeID containing the file types (i.e. AIFF, WAVE, etc) that can be opened for reading.
	/// @throw std::system_error.
	[[nodiscard]] static std::expected<std::vector<AudioFileTypeID>, OSStatus> ReadableTypes() noexcept;

	/// Returns an array of AudioFileTypeID containing the file types (i.e. AIFF, WAVE, etc) that can be opened for writing.
	/// @throw std::system_error.
	[[nodiscard]] static std::expected<std::vector<AudioFileTypeID>, OSStatus> WritableTypes() noexcept;

	/// Returns the name of type
	[[nodiscard]] static std::expected<CXXCFRef::CFString, OSStatus> CopyFileTypeName(AudioFileTypeID type) noexcept;

	/// Returns an array of supported formats for the fileType and formatID combination
	/// @throw std::system_error.
	[[nodiscard]] static std::expected<std::vector<CXXCoreAudio::CAStreamDescription>, OSStatus> AvailableStreamDescriptions(AudioFileTypeID fileType, AudioFormatID formatID) noexcept;

	/// Returns an array of format IDs that can be read.
	/// @throw std::system_error.
	[[nodiscard]] static std::expected<std::vector<AudioFormatID>, OSStatus> AvailableFormatIDs(AudioFileTypeID type) noexcept;


	/// Returns an array of recognized file extensions
	[[nodiscard]] static std::expected<CXXCFRef::CFArray, OSStatus> CopyAllExtensions() noexcept;

	/// Returns an array of recognized UTIs
	[[nodiscard]] static std::expected<CXXCFRef::CFArray, OSStatus> CopyAllUTIs() noexcept;

	/// Returns an array of recognized MIME types
	[[nodiscard]] static std::expected<CXXCFRef::CFArray, OSStatus> CopyAllMIMETypes() noexcept;


	/// Returns an array of file extensions for type
	[[nodiscard]] static std::expected<CXXCFRef::CFArray, OSStatus> CopyExtensionsForType(AudioFileTypeID type) noexcept;

	/// Returns an array of UTIs for type
	[[nodiscard]] static std::expected<CXXCFRef::CFArray, OSStatus> CopyUTIsForType(AudioFileTypeID type) noexcept;

	/// Returns an array of MIME types for type
	[[nodiscard]] static std::expected<CXXCFRef::CFArray, OSStatus> CopyMIMETypesForType(AudioFileTypeID type) noexcept;


	/// Returns an array of AudioFileTypeID that support mimeType
	/// @throw std::system_error.
	[[nodiscard]] static std::expected<std::vector<AudioFileTypeID>, OSStatus> TypesForMIMEType(CFStringRef mimeType) noexcept;

	/// Returns an array of AudioFileTypeID that support uti
	/// @throw std::system_error.
	[[nodiscard]] static std::expected<std::vector<AudioFileTypeID>, OSStatus> TypesForUTI(CFStringRef uti) noexcept;

	/// Returns an array of AudioFileTypeID that support extension
	/// @throw std::system_error.
	[[nodiscard]] static std::expected<std::vector<AudioFileTypeID>, OSStatus> TypesForExtension(CFStringRef extension) noexcept;


	/// Returns the managed AudioFile object.
	[[nodiscard]] AudioFileID _Nullable get() const noexcept;

	/// Replaces the managed AudioFile object with another AudioFile object.
	/// @note The object assumes responsibility for closing the passed AudioFile object using AudioFileClose.
	void reset(AudioFileID _Nullable audioFile = nullptr) noexcept;

	/// Swaps the managed AudioFile object with the managed AudioFile object from another audio file.
	void swap(CAAudioFile& other) noexcept;

	/// Releases ownership of the managed AudioFile object and returns it.
	/// @note The caller assumes responsibility for closing the returned AudioFile object using AudioFileClose.
	[[nodiscard]] AudioFileID _Nullable release() noexcept;

private:
	/// The managed AudioFile object.
	AudioFileID _Nullable audioFile_{nullptr};
};

// MARK: - Implementation -

inline CAAudioFile::operator bool() const noexcept
{
	return audioFile_ != nullptr;
}

inline CAAudioFile::operator AudioFileID const _Nullable () const noexcept
{
	return audioFile_;
}

inline AudioFileID _Nullable CAAudioFile::get() const noexcept
{
	return audioFile_;
}

inline void CAAudioFile::reset(AudioFileID _Nullable audioFile) noexcept
{
	if(auto old = std::exchange(audioFile_, audioFile); old)
		AudioFileClose(old);
}

inline void CAAudioFile::swap(CAAudioFile& other) noexcept
{
	std::swap(audioFile_, other.audioFile_);
}

inline AudioFileID _Nullable CAAudioFile::release() noexcept
{
	return std::exchange(audioFile_, nullptr);
}

} /* namespace CXXAudioToolbox */

CF_ASSUME_NONNULL_END
