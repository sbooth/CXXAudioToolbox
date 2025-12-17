//
// Copyright © 2024-2025 Stephen F. Booth
// Part of https://github.com/sbooth/CXXAudioToolbox
// MIT license
//

#pragma once

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

	/// Returns true if this object's internal AudioConverter object is not null.
	explicit operator bool() const noexcept
	{
		return converter_ != nullptr;
	}

	/// Returns the object's internal AudioConverter object.
	operator AudioConverterRef const _Nullable () const noexcept
	{
		return converter_;
	}

	/// Creates a new audio converter.
	/// @throw std::system_error.
	void New(const AudioStreamBasicDescription& inSourceFormat, const AudioStreamBasicDescription& inDestinationFormat);

	/// Creates a new audio converter using specific codecs.
	/// @throw std::system_error.
	void NewSpecific(const AudioStreamBasicDescription& inSourceFormat, const AudioStreamBasicDescription& inDestinationFormat, UInt32 inNumberClassDescriptions, const AudioClassDescription *inClassDescriptions);

	/// Destroys an existing audio converter.
	/// @throw std::system_error.
	void Dispose();

	/// Resets the audio converter.
	/// @throw std::system_error.
	void Reset();

	/// Returns information about an audio converter property.
	/// @throw std::system_error.
	void GetPropertyInfo(AudioConverterPropertyID inPropertyID, UInt32 * _Nullable outSize, Boolean * _Nullable outWritable);

	/// Returns an audio converter property value.
	/// @throw std::system_error.
	void GetProperty(AudioConverterPropertyID inPropertyID, UInt32& ioPropertyDataSize, void *outPropertyData);

	/// Sets an audio converter property value.
	/// @throw std::system_error.
	void SetProperty(AudioConverterPropertyID inPropertyID, UInt32 inPropertyDataSize, const void *inPropertyData);

	/// Converts data from an input buffer to an output buffer.
	/// @throw std::system_error.
	void ConvertBuffer(UInt32 inInputDataSize, const void *inInputData, UInt32& ioOutputDataSize, void *outOutputData);

	/// Converts data supplied by an input callback function, supporting non-interleaved and packetized formats.
	/// @throw std::system_error.
	void FillComplexBuffer(AudioConverterComplexInputDataProc inInputDataProc, void * _Nullable inInputDataProcUserData, UInt32& ioOutputDataPacketSize, AudioBufferList *outOutputData, AudioStreamPacketDescription * _Nullable outPacketDescription);

	/// Converts PCM data from an input buffer list to an output buffer list.
	/// @throw std::system_error.
	void ConvertComplexBuffer(UInt32 inNumberPCMFrames, const AudioBufferList *inInputData, AudioBufferList *outOutputData);


	/// Disposes of the internal AudioConverter object and replaces it with converter.
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

	/// Releases ownership of the internal AudioConverter object and returns it.
	AudioConverterRef _Nullable release() noexcept
	{
		return std::exchange(converter_, nullptr);
	}

private:
	/// The underlying AudioConverter object.
	AudioConverterRef _Nullable converter_{nullptr};
};

} /* namespace CXXAudioToolbox */

CF_ASSUME_NONNULL_END
