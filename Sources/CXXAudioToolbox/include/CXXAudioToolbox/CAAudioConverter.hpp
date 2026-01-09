//
// SPDX-FileCopyrightText: 2024 Stephen F. Booth <contact@sbooth.dev>
// SPDX-License-Identifier: MIT
//
// Part of https://github.com/sbooth/CXXAudioToolbox
//

#pragma once

#import <expected>
#import <utility>

#import <AudioToolbox/AudioConverter.h>

CF_ASSUME_NONNULL_BEGIN

namespace CXXAudioToolbox {

/// An AudioConverter wrapper.
class CAAudioConverter final {
public:
	/// Creates an audio converter.
	CAAudioConverter() noexcept = default;

	// This class is non-copyable
	CAAudioConverter(const CAAudioConverter&) = delete;

	// This class is non-assignable
	CAAudioConverter& operator=(const CAAudioConverter&) = delete;

	/// Move constructor.
	CAAudioConverter(CAAudioConverter&& other) noexcept;

	/// Move assignment operator.
	CAAudioConverter& operator=(CAAudioConverter&& other) noexcept;

	/// Destroys the audio converter and releases all associated resources.
	~CAAudioConverter() noexcept;


	/// Returns true if the managed AudioConverter object is not null.
	explicit operator bool() const noexcept
	{
		return converter_ != nullptr;
	}

	/// Returns the managed AudioConverter object.
	operator AudioConverterRef const _Nullable () const noexcept
	{
		return converter_;
	}

	
	/// Creates a new audio converter.
	std::expected<void, OSStatus> New(const AudioStreamBasicDescription& inSourceFormat, const AudioStreamBasicDescription& inDestinationFormat) noexcept;

	/// Creates a new audio converter using specific codecs.
	std::expected<void, OSStatus> NewSpecific(const AudioStreamBasicDescription& inSourceFormat, const AudioStreamBasicDescription& inDestinationFormat, UInt32 inNumberClassDescriptions, const AudioClassDescription *inClassDescriptions) noexcept;

	/// Destroys an existing audio converter.
	std::expected<void, OSStatus> Dispose() noexcept;

	/// Resets the audio converter.
	std::expected<void, OSStatus> Reset() noexcept;

	/// Returns information about an audio converter property.
	std::expected<void, OSStatus> GetPropertyInfo(AudioConverterPropertyID inPropertyID, UInt32 * _Nullable outSize, Boolean * _Nullable outWritable) noexcept;

	/// Returns an audio converter property value.
	std::expected<void, OSStatus> GetProperty(AudioConverterPropertyID inPropertyID, UInt32& ioPropertyDataSize, void *outPropertyData) noexcept;

	/// Sets an audio converter property value.
	std::expected<void, OSStatus> SetProperty(AudioConverterPropertyID inPropertyID, UInt32 inPropertyDataSize, const void *inPropertyData) noexcept;

	/// Converts data from an input buffer to an output buffer.
	std::expected<void, OSStatus> ConvertBuffer(UInt32 inInputDataSize, const void *inInputData, UInt32& ioOutputDataSize, void *outOutputData) noexcept;

	/// Converts data supplied by an input callback function, supporting non-interleaved and packetized formats.
	std::expected<void, OSStatus> FillComplexBuffer(AudioConverterComplexInputDataProc inInputDataProc, void * _Nullable inInputDataProcUserData, UInt32& ioOutputDataPacketSize, AudioBufferList *outOutputData, AudioStreamPacketDescription * _Nullable outPacketDescription) noexcept;

	/// Converts PCM data from an input buffer list to an output buffer list.
	std::expected<void, OSStatus> ConvertComplexBuffer(UInt32 inNumberPCMFrames, const AudioBufferList *inInputData, AudioBufferList *outOutputData) noexcept;


	/// Returns the managed AudioConverter object.
	AudioConverterRef _Nullable get() const noexcept
	{
		return converter_;
	}

	/// Replaces the managed AudioConverter object with another AudioConverter object.
	/// @note The object assumes responsibility for disposing of the passed AudioConverter object using AudioConverterDispose.
	void reset(AudioConverterRef _Nullable converter = nullptr) noexcept
	{
		if(auto old = std::exchange(converter_, converter); old)
			AudioConverterDispose(old);
	}

	/// Swaps the managed AudioConverter object with the managed AudioConverter object from another audio converter.
	void swap(CAAudioConverter& other) noexcept
	{
		std::swap(converter_, other.converter_);
	}

	/// Releases ownership of the managed AudioConverter object and returns it.
	/// @note The caller assumes responsibility for disposing of the returned AudioConverter object using AudioConverterDispose.
	AudioConverterRef _Nullable release() noexcept
	{
		return std::exchange(converter_, nullptr);
	}

private:
	/// The managed AudioConverter object.
	AudioConverterRef _Nullable converter_{nullptr};
};

} /* namespace CXXAudioToolbox */

CF_ASSUME_NONNULL_END
