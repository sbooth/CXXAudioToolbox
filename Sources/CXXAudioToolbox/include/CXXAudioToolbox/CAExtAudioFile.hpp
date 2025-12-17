//
// Copyright © 2021-2025 Stephen F. Booth
// Part of https://github.com/sbooth/CXXAudioToolbox
// MIT license
//

#pragma once

#import <utility>
#import <stdexcept>

#import <AudioToolbox/AudioConverter.h>
#import <AudioToolbox/ExtendedAudioFile.h>

#ifdef __OBJC__
#import <AVFAudio/AVFAudio.h>
#endif /* __OBJC__ */

#import <CXXCoreAudio/CAAudioBuffer.hpp>
#import <CXXCoreAudio/CAChannelLayout.hpp>
#import <CXXCoreAudio/CAStreamDescription.hpp>

CF_ASSUME_NONNULL_BEGIN

namespace CXXAudioToolbox {

/// An ExtAudioFIle wrapper.
class CAExtAudioFile {
public:
	/// Creates an extended audio file.
	CAExtAudioFile() noexcept = default;

	// This class is non-copyable
	CAExtAudioFile(const CAExtAudioFile&) = delete;

	// This class is non-assignable
	CAExtAudioFile& operator=(const CAExtAudioFile&) = delete;

	/// Move constructor.
	CAExtAudioFile(CAExtAudioFile&& other) noexcept;

	/// Move assignment operator.
	CAExtAudioFile& operator=(CAExtAudioFile&& other) noexcept;

	/// Destroys the extended audio file and releases all associated resources.
	~CAExtAudioFile() noexcept;


	/// Returns true if this object's internal ExtAudioFile object is not null.
	explicit operator bool() const noexcept
	{
		return extAudioFile_ != nullptr;
	}

	/// Returns the file's internal ExtAudioFile object.
	operator ExtAudioFileRef const _Nullable () const noexcept
	{
		return extAudioFile_;
	}


	/// Opens an audio file specified by a CFURLRef.
	///
	/// Allocates a new ExtAudioFileRef, for reading an existing audio file.
	/// @param inURL The audio file to read.
	/// @throw std::system_error.
	void OpenURL(CFURLRef inURL);

	/// Wraps an AudioFileID in an ExtAudioFileRef.
	///
	/// Allocates a new ExtAudioFileRef which wraps an existing AudioFileID. The
	/// client is responsible for keeping the AudioFileID open until the
	/// ExtAudioFileRef is disposed. Disposing the ExtAudioFileRef will not close
	/// the AudioFileID when this Wrap API call is used, so the client is also
	/// responsible for closing the AudioFileID when finished with it.
	///
	/// @param inFileID The AudioFileID to wrap
	/// @param inForWriting true if the AudioFileID is a new file opened for writing
	/// @throw std::system_error.
	void WrapAudioFileID(AudioFileID inFileID, bool inForWriting);

	/// Creates a new audio file.
	///
	/// If the file to be created is in an encoded format, it is permissible for the
	/// sample rate in inStreamDesc to be 0, since in all cases, the file's encoding
	/// AudioConverter may produce audio at a different sample rate than the source. The
	/// file will be created with the audio format actually produced by the encoder.
	///
	/// @param inURL The URL of the new audio file.
	/// @param inFileType The type of file to create. This is a constant from AudioToolbox/AudioFile.h, e.g.
	/// kAudioFileAIFFType. Note that this is not an HFSTypeCode.
	/// @param inStreamDesc The format of the audio data to be written to the file.
	/// @param inChannelLayout The channel layout of the audio data. If non-null, this must be consistent
	/// with the number of channels specified by inStreamDesc.
	/// @param inFlags The same flags as are used with AudioFileCreateWithURL
	/// Can use these to control whether an existing file is overwritten (or not).
	/// @throw std::system_error.
	void CreateWithURL(CFURLRef inURL, AudioFileTypeID inFileType, const AudioStreamBasicDescription& inStreamDesc, const AudioChannelLayout * _Nullable const inChannelLayout, UInt32 inFlags);

	/// Closes the file and disposes of the internal extended audio file.
	/// @throw std::system_error.
	void Dispose();

	/// Performs a synchronous sequential read.
	///
	/// If the file has a client data format, then the audio data from the file is
	/// translated from the file data format to the client format, via the
	/// ExtAudioFile's internal AudioConverter.
	///
	/// (Note that the use of sequential reads/writes means that an ExtAudioFile must
	/// not be read on multiple threads; clients wishing to do this should use the
	/// lower-level AudioFile API set).
	///
	/// @param ioNumberFrames On entry, ioNumberFrames is the number of frames to be read from the file.
	/// On exit, it is the number of frames actually read. A number of factors may
	/// cause a fewer number of frames to be read, including the supplied buffers
	/// not being large enough, and internal optimizations. If 0 frames are
	/// returned, however, this indicates that end-of-file was reached.
	/// @param ioData Buffer(s) into which the audio data is read.
	/// @throw std::system_error.
	void Read(UInt32& ioNumberFrames, AudioBufferList *ioData);

	/// Performs a synchronous sequential read.
	/// @param buffer Buffer into which the audio data is read.
	/// @throw std::system_error.
	void Read(CXXCoreAudio::CAAudioBuffer& buffer);

	/// Performs a synchronous sequential write.
	///
	///	If the file has a client data format, then the audio data in ioData is
	/// translated from the client format to the file data format, via the
	/// ExtAudioFile's internal AudioConverter.
	///
	/// @param inNumberFrames The number of frames to write.
	/// @param ioData The buffer(s) from which audio data is written to the file.
	/// @throw std::system_error.
#if TARGET_OS_IPHONE
	OSStatus Write(UInt32 inNumberFrames, const AudioBufferList *ioData);
#else
	void Write(UInt32 inNumberFrames, const AudioBufferList *ioData);
#endif /* TARGET_OS_IPHONE */

	/// Performs an asynchronous sequential write.
	///
	/// Writes the provided buffer list to an internal ring buffer and notifies an
	/// internal thread to perform the write at a later time. The first time this is
	/// called, allocations may be performed. You can call this with 0 frames and null
	/// buffer in a non-time-critical context to initialize the asynchronous mechanism.
	/// Once initialized, subsequent calls are very efficient and do not take locks;
	/// thus this may be used to write to a file from a realtime thread.
	///
	/// The client must not mix synchronous and asynchronous writes to the same file.
	///
	/// Pending writes are not guaranteed to be flushed to disk until
	/// ExtAudioFileDispose is called.
	///
	/// N.B. Errors may occur after this call has returned. Such errors may be returned
	/// from subsequent calls to this function.
	///
	/// @param inNumberFrames The number of frames to write.
	/// @param ioData The buffer(s) from which audio data is written to the file.
	/// @throw std::system_error.
	void WriteAsync(UInt32 inNumberFrames, const AudioBufferList * _Nullable ioData);

	/// Seeks to a specific frame position.
	///
	/// Sets the file's read position to the specified sample frame number. The next call
	/// to ExtAudioFileRead will return samples from precisely this location, even if it
	/// is located in the middle of a packet.
	///
	/// This function's behavior with files open for writing is currently undefined.
	///
	/// @param inFrameOffset The desired seek position, in sample frames, relative to the beginning of
	/// the file. This is specified in the sample rate and frame count of the file's format
	/// (not the client format).
	/// @throw std::system_error.
	void Seek(SInt64 inFrameOffset);

	/// Returns the file's read/write position.
	/// @return The file's current read/write position in sample frames. This is specified in the
	/// sample rate and frame count of the file's format (not the client format).
	/// @throw std::system_error.
	SInt64 Tell() const;

	/// Gets information about a property.
	/// @param inPropertyID The property being queried.
	/// @param outWritable If non-null, on exit, this indicates whether the property value is settable.
	/// @return The size of the property's value.
	/// @throw std::system_error.
	UInt32 GetPropertyInfo(ExtAudioFilePropertyID inPropertyID, Boolean * _Nullable outWritable) const;

	/// Gets a property value.
	/// @param inPropertyID The property being fetched.
	/// @param ioPropertyDataSize On entry, the size (in bytes) of the memory pointed to by outPropertyData.
	/// On exit, the actual size of the property data returned.
	/// @param outPropertyData The value of the property is copied to the memory this points to.
	/// @throw std::system_error.
	void GetProperty(ExtAudioFilePropertyID inPropertyID, UInt32& ioPropertyDataSize, void *outPropertyData) const;

	/// Sets a property value.
	/// @param inPropertyID The property being set.
	/// @param inPropertyDataSize The size of the property data, in bytes.
	/// @param inPropertyData Points to the property's new value.
	/// @throw std::system_error.
	void SetProperty(ExtAudioFilePropertyID inPropertyID, UInt32 inPropertyDataSize, const void *inPropertyData);

	/// Returns the file's channel layout (kExtAudioFileProperty_FileChannelLayout).
	/// @throw std::system_error.
	/// @throw std::bad_alloc.
	CXXCoreAudio::CAChannelLayout FileChannelLayout() const;

	/// Sets the file's channel layout (kExtAudioFileProperty_FileChannelLayout).
	/// @throw std::system_error.
	void SetFileChannelLayout(const AudioChannelLayout& fileChannelLayout);

	/// Returns the file's data format (kExtAudioFileProperty_FileDataFormat).
	/// @throw std::system_error.
	CXXCoreAudio::CAStreamDescription FileDataFormat() const;

	/// Returns the client data format (kExtAudioFileProperty_ClientDataFormat).
	/// @throw std::system_error.
	CXXCoreAudio::CAStreamDescription ClientDataFormat() const;

	/// Sets the client data format (kExtAudioFileProperty_ClientDataFormat).
	/// @throw std::system_error.
	void SetClientDataFormat(const AudioStreamBasicDescription& clientDataFormat, const AudioChannelLayout * const _Nullable clientChannelLayout = nullptr, UInt32 codecManufacturer = 0);

	/// Returns the client channel layout (kExtAudioFileProperty_ClientChannelLayout).
	/// @throw std::system_error.
	/// @throw std::bad_alloc
	CXXCoreAudio::CAChannelLayout ClientChannelLayout() const;

	/// Sets the client channel layout (kExtAudioFileProperty_ClientChannelLayout).
	/// @throw std::system_error.
	void SetClientChannelLayout(const AudioChannelLayout& clientChannelLayout);

	/// Returns the internal AudioConverter (kExtAudioFileProperty_AudioConverter).
	/// @throw std::system_error.
	AudioConverterRef _Nullable AudioConverter() const;

	/// Returns true if the extended audio file has an internal audio converter.
	bool HasAudioConverter() const
	{
		return AudioConverter() != nullptr;
	}

	/// Sets a property on the internal audio converter.
	void SetAudioConverterProperty(AudioConverterPropertyID inPropertyID, UInt32 inPropertyDataSize, const void *inPropertyData);

	/// Returns the length of the file in audio frames (kExtAudioFileProperty_FileLengthFrames).
	/// @throw std::system_error.
	SInt64 FrameLength() const;

#ifdef __OBJC__
	/// Returns the file's data format (kExtAudioFileProperty_FileDataFormat) and channel layout (kExtAudioFileProperty_FileChannelLayout).
	/// @throw std::system_error.
	/// @throw std::runtime_error.
	/// @throw std::bad_alloc
	AVAudioFormat * FileFormat() const
	{
		const auto dataFormat = FileDataFormat();
		const auto channelLayout = FileChannelLayout();
		if(dataFormat.ChannelCount() > 2 && !channelLayout)
			throw std::runtime_error("File data format > 2 channels with no file channel layout");
		return [[AVAudioFormat alloc] initWithStreamDescription:&dataFormat channelLayout:channelLayout];
	}

	/// Returns the client data format (kExtAudioFileProperty_ClientDataFormat) and channel layout (kExtAudioFileProperty_ClientChannelLayout).
	/// @throw std::system_error.
	/// @throw std::runtime_error.
	/// @throw std::bad_alloc
	AVAudioFormat * ClientFormat() const
	{
		const auto dataFormat = ClientDataFormat();
		const auto channelLayout = ClientChannelLayout();
		if(dataFormat.ChannelCount() > 2 && !channelLayout)
			throw std::runtime_error("Client data format > 2 channels with no client channel layout");
		return [[AVAudioFormat alloc] initWithStreamDescription:&dataFormat channelLayout:channelLayout];
	}

	/// Sets the client data format (kExtAudioFileProperty_ClientDataFormat) and channel layout (kExtAudioFileProperty_ClientChannelLayout).
	/// @throw std::system_error.
	/// @throw std::bad_alloc
	void SetClientFormat(AVAudioFormat *format)
	{
		NSCParameterAssert(format != nil);
		SetClientDataFormat(*format.streamDescription);
		SetClientChannelLayout(*format.channelLayout.layout);
	}
#endif /* __OBJC__ */


	/// Returns the managed ExtAudioFile object.
	ExtAudioFileRef _Nullable get() const noexcept
	{
		return extAudioFile_;
	}

	/// Disposes of the internal ExtAudioFile object and replaces it with extAudioFile.
	void reset(ExtAudioFileRef _Nullable extAudioFile = nullptr) noexcept
	{
		if(auto old = std::exchange(extAudioFile_, extAudioFile); old)
			ExtAudioFileDispose(old);
	}

	/// Swaps the managed ExtAudioFile object with the managed ExtAudioFile object from another audio converter.
	void swap(CAExtAudioFile& other) noexcept
	{
		std::swap(extAudioFile_, other.extAudioFile_);
	}

	/// Releases ownership of the internal ExtAudioFile object and returns it.
	ExtAudioFileRef _Nullable release() noexcept
	{
		return std::exchange(extAudioFile_, nullptr);
	}

private:
	/// The underlying ExtAudioFile object.
	ExtAudioFileRef _Nullable extAudioFile_{nullptr};
};

} /* namespace CXXAudioToolbox */

CF_ASSUME_NONNULL_END
