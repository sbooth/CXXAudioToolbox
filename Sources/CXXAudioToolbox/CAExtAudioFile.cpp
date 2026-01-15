//
// SPDX-FileCopyrightText: 2021 Stephen F. Booth <contact@sbooth.dev>
// SPDX-License-Identifier: MIT
//
// Part of https://github.com/sbooth/CXXAudioToolbox
//

#import "CAExtAudioFile.hpp"

namespace {

struct free_deleter {
	template <typename T>
	void operator()(T * _Nonnull ptr) const {
		std::free(const_cast<std::remove_const_t<T> *>(ptr));
	}
};

} /* namespace */

CXXAudioToolbox::CAExtAudioFile::~CAExtAudioFile() noexcept
{
	reset();
}

CXXAudioToolbox::CAExtAudioFile::CAExtAudioFile(CAExtAudioFile&& other) noexcept
: extAudioFile_{other.release()}
{}

CXXAudioToolbox::CAExtAudioFile& CXXAudioToolbox::CAExtAudioFile::operator=(CAExtAudioFile&& other) noexcept
{
	reset(other.release());
	return *this;
}

std::expected<void, OSStatus> CXXAudioToolbox::CAExtAudioFile::OpenURL(CFURLRef inURL) noexcept
{
	const auto ext_audio_file_open_url = [&] -> std::expected<void, OSStatus> {
		const auto result = ExtAudioFileOpenURL(inURL, &extAudioFile_);
		if(result != noErr)
			return std::unexpected(result);
		return {};
	};

	return Dispose().and_then(ext_audio_file_open_url);
}

std::expected<void, OSStatus> CXXAudioToolbox::CAExtAudioFile::WrapAudioFileID(AudioFileID inFileID, bool inForWriting) noexcept
{
	const auto ext_audio_file_wrap_audio_file_id = [&] -> std::expected<void, OSStatus> {
		const auto result = ExtAudioFileWrapAudioFileID(inFileID, inForWriting, &extAudioFile_);
		if(result != noErr)
			return std::unexpected(result);
		return {};
	};

	return Dispose().and_then(ext_audio_file_wrap_audio_file_id);
}

std::expected<void, OSStatus> CXXAudioToolbox::CAExtAudioFile::CreateWithURL(CFURLRef inURL, AudioFileTypeID inFileType, const AudioStreamBasicDescription& inStreamDesc, const AudioChannelLayout * _Nullable const inChannelLayout, UInt32 inFlags) noexcept
{
	const auto ext_audio_file_create_with_url = [&] -> std::expected<void, OSStatus> {
		const auto result = ExtAudioFileCreateWithURL(inURL, inFileType, &inStreamDesc, inChannelLayout, inFlags, &extAudioFile_);
		if(result != noErr)
			return std::unexpected(result);
		return {};
	};

	return Dispose().and_then(ext_audio_file_create_with_url);
}

std::expected<void, OSStatus> CXXAudioToolbox::CAExtAudioFile::Dispose() noexcept
{
	if(extAudioFile_) {
		const auto result = ExtAudioFileDispose(extAudioFile_);
		extAudioFile_ = nullptr;
		if(result != noErr)
			return std::unexpected(result);
	}
	return {};
}

std::expected<void, OSStatus> CXXAudioToolbox::CAExtAudioFile::Read(UInt32& ioNumberFrames, AudioBufferList *ioData) noexcept
{
	const auto result = ExtAudioFileRead(extAudioFile_, &ioNumberFrames, ioData);
	if(result != noErr)
		return std::unexpected(result);
	return {};
}

std::expected<void, OSStatus> CXXAudioToolbox::CAExtAudioFile::Read(CXXCoreAudio::CAAudioBuffer& buffer) noexcept
{
	buffer.PrepareForReading();
	UInt32 frameCount = buffer.FrameCapacity();
	const auto result = ExtAudioFileRead(extAudioFile_, &frameCount, buffer);
	if(result != noErr)
		return std::unexpected(result);
	buffer.SetFrameLength(frameCount);
	return {};
}

#if TARGET_OS_IPHONE
std::expected<OSStatus, OSStatus> CXXAudioToolbox::CAExtAudioFile::Write(UInt32 inNumberFrames, const AudioBufferList *ioData) noexcept
#else
std::expected<void, OSStatus> CXXAudioToolbox::CAExtAudioFile::Write(UInt32 inNumberFrames, const AudioBufferList *ioData) noexcept
#endif /* TARGET_OS_IPHONE */
{
	const auto result = ExtAudioFileWrite(extAudioFile_, inNumberFrames, ioData);
#if TARGET_OS_IPHONE
	switch(result) {
		case noErr:
		case kExtAudioFileError_CodecUnavailableInputConsumed:
		case kExtAudioFileError_CodecUnavailableInputNotConsumed:
			return result;
		default:
			return std::unexpected(result);
			break;
	}
#else
	if(result != noErr)
		return std::unexpected(result);
	return {};
#endif /* TARGET_OS_IPHONE */
}

std::expected<void, OSStatus> CXXAudioToolbox::CAExtAudioFile::WriteAsync(UInt32 inNumberFrames, const AudioBufferList * _Nullable ioData) noexcept
{
	const auto result = ExtAudioFileWriteAsync(extAudioFile_, inNumberFrames, ioData);
	if(result != noErr)
		return std::unexpected(result);
	return {};
}

std::expected<void, OSStatus> CXXAudioToolbox::CAExtAudioFile::Seek(SInt64 inFrameOffset) noexcept
{
	const auto result = ExtAudioFileSeek(extAudioFile_, inFrameOffset);
	if(result != noErr)
		return std::unexpected(result);
	return {};
}

std::expected<SInt64, OSStatus> CXXAudioToolbox::CAExtAudioFile::Tell() const noexcept
{
	SInt64 pos;
	const auto result = ExtAudioFileTell(extAudioFile_, &pos);
	if(result != noErr)
		return std::unexpected(result);
	return pos;
}

std::expected<void, OSStatus> CXXAudioToolbox::CAExtAudioFile::GetPropertyInfo(ExtAudioFilePropertyID inPropertyID, UInt32 * _Nullable outSize, Boolean * _Nullable outWritable) const
{
	const auto result = ExtAudioFileGetPropertyInfo(extAudioFile_, inPropertyID, outSize, outWritable);
	if(result != noErr)
		return std::unexpected(result);
	return {};
}

std::expected<void, OSStatus> CXXAudioToolbox::CAExtAudioFile::GetProperty(ExtAudioFilePropertyID inPropertyID, UInt32& ioPropertyDataSize, void *outPropertyData) const noexcept
{
	const auto result = ExtAudioFileGetProperty(extAudioFile_, inPropertyID, &ioPropertyDataSize, outPropertyData);
	if(result != noErr)
		return std::unexpected(result);
	return {};
}

std::expected<void, OSStatus> CXXAudioToolbox::CAExtAudioFile::SetProperty(ExtAudioFilePropertyID inPropertyID, UInt32 inPropertyDataSize, const void *inPropertyData) noexcept
{
	const auto result = ExtAudioFileSetProperty(extAudioFile_, inPropertyID, inPropertyDataSize, inPropertyData);
	if(result != noErr)
		return std::unexpected(result);
	return {};
}

std::expected<CXXCoreAudio::CAChannelLayout, OSStatus> CXXAudioToolbox::CAExtAudioFile::FileChannelLayout() const noexcept
{
	UInt32 size;
	auto result = ExtAudioFileGetPropertyInfo(extAudioFile_, kExtAudioFileProperty_FileChannelLayout, &size, nullptr);
	if(result != noErr)
		return std::unexpected(result);

	std::unique_ptr<AudioChannelLayout, free_deleter> layout{static_cast<AudioChannelLayout *>(std::malloc(size))};
	if(!layout)
		return std::unexpected(kAudio_MemFullError);

	result = ExtAudioFileGetProperty(extAudioFile_, kExtAudioFileProperty_FileChannelLayout, &size, layout.get());
	if(result != noErr)
		return std::unexpected(result);

	CXXCoreAudio::CAChannelLayout channelLayout{};
	channelLayout.reset(layout.release());
	return channelLayout;
}

std::expected<void, OSStatus> CXXAudioToolbox::CAExtAudioFile::SetFileChannelLayout(const AudioChannelLayout& fileChannelLayout) noexcept
{
	const auto result = ExtAudioFileSetProperty(extAudioFile_, kExtAudioFileProperty_FileChannelLayout, static_cast<UInt32>(CXXCoreAudio::AudioChannelLayoutSize(&fileChannelLayout)), &fileChannelLayout);
	if(result != noErr)
		return std::unexpected(result);
	return {};
}

std::expected<CXXCoreAudio::CAStreamDescription, OSStatus> CXXAudioToolbox::CAExtAudioFile::FileDataFormat() const noexcept
{
	CXXCoreAudio::CAStreamDescription format;
	UInt32 size = sizeof format;
	const auto result = ExtAudioFileGetProperty(extAudioFile_, kExtAudioFileProperty_FileDataFormat, &size, &format);
	if(result != noErr)
		return std::unexpected(result);
	return format;
}

std::expected<CXXCoreAudio::CAStreamDescription, OSStatus> CXXAudioToolbox::CAExtAudioFile::ClientDataFormat() const noexcept
{
	CXXCoreAudio::CAStreamDescription format;
	UInt32 size = sizeof format;
	const auto result = ExtAudioFileGetProperty(extAudioFile_, kExtAudioFileProperty_ClientDataFormat, &size, &format);
	if(result != noErr)
		return std::unexpected(result);
	return format;
}

std::expected<void, OSStatus> CXXAudioToolbox::CAExtAudioFile::SetClientDataFormat(const AudioStreamBasicDescription& clientDataFormat) noexcept
{
	const auto result = ExtAudioFileSetProperty(extAudioFile_, kExtAudioFileProperty_ClientDataFormat, sizeof clientDataFormat, &clientDataFormat);
	if(result != noErr)
		return std::unexpected(result);
	return {};
}

std::expected<void, OSStatus> CXXAudioToolbox::CAExtAudioFile::SetClientDataFormat(const AudioStreamBasicDescription& clientDataFormat, const AudioChannelLayout& clientChannelLayout, UInt32 codecManufacturer) noexcept
{
	if(codecManufacturer) {
		const auto result = ExtAudioFileSetProperty(extAudioFile_, kExtAudioFileProperty_CodecManufacturer, sizeof codecManufacturer, &codecManufacturer);
		if(result != noErr)
			return std::unexpected(result);
	}

	auto result = ExtAudioFileSetProperty(extAudioFile_, kExtAudioFileProperty_ClientDataFormat, sizeof clientDataFormat, &clientDataFormat);
	if(result != noErr)
		return std::unexpected(result);

	result = ExtAudioFileSetProperty(extAudioFile_, kExtAudioFileProperty_ClientChannelLayout, static_cast<UInt32>(CXXCoreAudio::AudioChannelLayoutSize(&clientChannelLayout)), &clientChannelLayout);
	if(result != noErr)
		return std::unexpected(result);

	return {};
}

std::expected<CXXCoreAudio::CAChannelLayout, OSStatus> CXXAudioToolbox::CAExtAudioFile::ClientChannelLayout() const noexcept
{
	UInt32 size;
	auto result = ExtAudioFileGetPropertyInfo(extAudioFile_, kExtAudioFileProperty_ClientChannelLayout, &size, nullptr);
	if(result != noErr)
		return std::unexpected(result);

	std::unique_ptr<AudioChannelLayout, free_deleter> layout{static_cast<AudioChannelLayout *>(std::malloc(size))};
	if(!layout)
		return std::unexpected(kAudio_MemFullError);

	result = ExtAudioFileGetProperty(extAudioFile_, kExtAudioFileProperty_ClientChannelLayout, &size, layout.get());
	if(result != noErr)
		return std::unexpected(result);

	CXXCoreAudio::CAChannelLayout channelLayout{};
	channelLayout.reset(layout.release());
	return channelLayout;
}

std::expected<void, OSStatus> CXXAudioToolbox::CAExtAudioFile::SetClientChannelLayout(const AudioChannelLayout& clientChannelLayout) noexcept
{
	const auto result = ExtAudioFileSetProperty(extAudioFile_, kExtAudioFileProperty_ClientChannelLayout, static_cast<UInt32>(CXXCoreAudio::AudioChannelLayoutSize(&clientChannelLayout)), &clientChannelLayout);
	if(result != noErr)
		return std::unexpected(result);
	return {};
}

std::expected<AudioConverterRef, OSStatus> CXXAudioToolbox::CAExtAudioFile::AudioConverter() const noexcept
{
	AudioConverterRef converter = nullptr;
	UInt32 size = sizeof converter;
	const auto result = ExtAudioFileGetProperty(extAudioFile_, kExtAudioFileProperty_AudioConverter, &size, &converter);
	if(result != noErr)
		return std::unexpected(result);
	return converter;
}

std::expected<void, OSStatus> CXXAudioToolbox::CAExtAudioFile::SetAudioConverterProperty(AudioConverterPropertyID inPropertyID, UInt32 inPropertyDataSize, const void *inPropertyData) noexcept
{
	AudioConverterRef converter = nullptr;
	UInt32 size = sizeof converter;
	auto result = ExtAudioFileGetProperty(extAudioFile_, kExtAudioFileProperty_AudioConverter, &size, &converter);
	if(result != noErr)
		return std::unexpected(result);

	result = AudioConverterSetProperty(converter, inPropertyID, inPropertyDataSize, inPropertyData);
	if(result != noErr)
		return std::unexpected(result);

	CFPropertyListRef config = nullptr;
	result = ExtAudioFileSetProperty(extAudioFile_, kExtAudioFileProperty_ConverterConfig, sizeof config, &config);
	if(result != noErr)
		return std::unexpected(result);
	return {};
}

std::expected<SInt64, OSStatus> CXXAudioToolbox::CAExtAudioFile::FrameLength() const noexcept
{
	SInt64 frameLength;
	UInt32 size = sizeof frameLength;
	const auto result = ExtAudioFileGetProperty(extAudioFile_, kExtAudioFileProperty_FileLengthFrames, &size, &frameLength);
	if(result != noErr)
		return std::unexpected(result);
	return frameLength;
}
