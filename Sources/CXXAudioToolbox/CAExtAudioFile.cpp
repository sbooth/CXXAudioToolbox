//
// Copyright © 2021-2025 Stephen F. Booth
// Part of https://github.com/sbooth/CXXAudioToolbox
// MIT license
//

#import "CAExtAudioFile.hpp"
#import "AudioToolboxErrors.hpp"

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

void CXXAudioToolbox::CAExtAudioFile::OpenURL(CFURLRef inURL)
{
	Dispose();
	const auto result = ExtAudioFileOpenURL(inURL, &extAudioFile_);
	ThrowIfExtAudioFileError(result, "ExtAudioFileOpenURL");
}

void CXXAudioToolbox::CAExtAudioFile::WrapAudioFileID(AudioFileID inFileID, bool inForWriting)
{
	Dispose();
	const auto result = ExtAudioFileWrapAudioFileID(inFileID, inForWriting, &extAudioFile_);
	ThrowIfExtAudioFileError(result, "ExtAudioFileWrapAudioFileID");
}

void CXXAudioToolbox::CAExtAudioFile::CreateWithURL(CFURLRef inURL, AudioFileTypeID inFileType, const AudioStreamBasicDescription& inStreamDesc, const AudioChannelLayout * _Nullable const inChannelLayout, UInt32 inFlags)
{
	Dispose();
	const auto result = ExtAudioFileCreateWithURL(inURL, inFileType, &inStreamDesc, inChannelLayout, inFlags, &extAudioFile_);
	ThrowIfExtAudioFileError(result, "ExtAudioFileCreateWithURL");
}

void CXXAudioToolbox::CAExtAudioFile::Dispose()
{
	if(extAudioFile_) {
		const auto result = ExtAudioFileDispose(extAudioFile_);
		extAudioFile_ = nullptr;
		ThrowIfExtAudioFileError(result, "ExtAudioFileDispose");
	}
}

void CXXAudioToolbox::CAExtAudioFile::Read(UInt32& ioNumberFrames, AudioBufferList *ioData)
{
	const auto result = ExtAudioFileRead(extAudioFile_, &ioNumberFrames, ioData);
	ThrowIfExtAudioFileError(result, "ExtAudioFileRead");
}

void CXXAudioToolbox::CAExtAudioFile::Read(CXXCoreAudio::CAAudioBuffer& buffer)
{
	buffer.PrepareForReading();
	UInt32 frameCount = buffer.FrameCapacity();
	Read(frameCount, buffer);
	buffer.SetFrameLength(frameCount);
}

#if TARGET_OS_IPHONE
OSStatus CXXAudioToolbox::CAExtAudioFile::Write(UInt32 inNumberFrames, const AudioBufferList *ioData)
#else
void CXXAudioToolbox::CAExtAudioFile::Write(UInt32 inNumberFrames, const AudioBufferList *ioData)
#endif /* TARGET_OS_IPHONE */
{
	const auto result = ExtAudioFileWrite(extAudioFile_, inNumberFrames, ioData);
#if TARGET_OS_IPHONE
	switch(result) {
		case noErr:
		case kExtAudioFileError_CodecUnavailableInputConsumed:
		case kExtAudioFileError_CodecUnavailableInputNotConsumed:
			break;

		default:
			ThrowIfExtAudioFileError(result, "ExtAudioFileWrite");
			break;
	}
	return result;
#else
	ThrowIfExtAudioFileError(result, "ExtAudioFileWrite");
#endif /* TARGET_OS_IPHONE */
}

void CXXAudioToolbox::CAExtAudioFile::WriteAsync(UInt32 inNumberFrames, const AudioBufferList * _Nullable ioData)
{
	const auto result = ExtAudioFileWriteAsync(extAudioFile_, inNumberFrames, ioData);
	ThrowIfExtAudioFileError(result, "ExtAudioFileWriteAsync");
}

void CXXAudioToolbox::CAExtAudioFile::Seek(SInt64 inFrameOffset)
{
	const auto result = ExtAudioFileSeek(extAudioFile_, inFrameOffset);
	ThrowIfExtAudioFileError(result, "ExtAudioFileSeek");
}

SInt64 CXXAudioToolbox::CAExtAudioFile::Tell() const
{
	SInt64 pos;
	const auto result = ExtAudioFileTell(extAudioFile_, &pos);
	ThrowIfExtAudioFileError(result, "ExtAudioFileTell");
	return pos;
}

UInt32 CXXAudioToolbox::CAExtAudioFile::GetPropertyInfo(ExtAudioFilePropertyID inPropertyID, Boolean * _Nullable outWritable) const
{
	UInt32 size;
	const auto result = ExtAudioFileGetPropertyInfo(extAudioFile_, inPropertyID, &size, outWritable);
	ThrowIfExtAudioFileError(result, "ExtAudioFileGetPropertyInfo");
	return size;
}

void CXXAudioToolbox::CAExtAudioFile::GetProperty(ExtAudioFilePropertyID inPropertyID, UInt32& ioPropertyDataSize, void *outPropertyData) const
{
	const auto result = ExtAudioFileGetProperty(extAudioFile_, inPropertyID, &ioPropertyDataSize, outPropertyData);
	ThrowIfExtAudioFileError(result, "ExtAudioFileGetProperty");
}

void CXXAudioToolbox::CAExtAudioFile::SetProperty(ExtAudioFilePropertyID inPropertyID, UInt32 inPropertyDataSize, const void *inPropertyData)
{
	const auto result = ExtAudioFileSetProperty(extAudioFile_, inPropertyID, inPropertyDataSize, inPropertyData);
	ThrowIfExtAudioFileError(result, "ExtAudioFileSetProperty");
}

CXXCoreAudio::CAChannelLayout CXXAudioToolbox::CAExtAudioFile::FileChannelLayout() const
{
	auto size = GetPropertyInfo(kExtAudioFileProperty_FileChannelLayout, nullptr);
	std::unique_ptr<AudioChannelLayout, free_deleter> layout{static_cast<AudioChannelLayout *>(std::malloc(size))};
	if(!layout)
		throw std::bad_alloc();
	GetProperty(kExtAudioFileProperty_FileChannelLayout, size, layout.get());
	CXXCoreAudio::CAChannelLayout channelLayout{};
	channelLayout.Reset(layout.release());
	return channelLayout;
}

void CXXAudioToolbox::CAExtAudioFile::SetFileChannelLayout(const AudioChannelLayout& fileChannelLayout)
{
	SetProperty(kExtAudioFileProperty_FileChannelLayout, static_cast<UInt32>(CXXCoreAudio::AudioChannelLayoutSize(&fileChannelLayout)), &fileChannelLayout);
}

CXXCoreAudio::CAStreamDescription CXXAudioToolbox::CAExtAudioFile::FileDataFormat() const
{
	CXXCoreAudio::CAStreamDescription fileDataFormat;
	UInt32 size = sizeof fileDataFormat;
	GetProperty(kExtAudioFileProperty_FileDataFormat, size, &fileDataFormat);
	return fileDataFormat;
}

CXXCoreAudio::CAStreamDescription CXXAudioToolbox::CAExtAudioFile::ClientDataFormat() const
{
	CXXCoreAudio::CAStreamDescription clientDataFormat;
	UInt32 size = sizeof clientDataFormat;
	GetProperty(kExtAudioFileProperty_ClientDataFormat, size, &clientDataFormat);
	return clientDataFormat;
}

void CXXAudioToolbox::CAExtAudioFile::SetClientDataFormat(const AudioStreamBasicDescription& clientDataFormat, const AudioChannelLayout * const clientChannelLayout, UInt32 codecManufacturer)
{
	if(codecManufacturer)
		SetProperty(kExtAudioFileProperty_CodecManufacturer, sizeof codecManufacturer, &codecManufacturer);
	SetProperty(kExtAudioFileProperty_ClientDataFormat, sizeof clientDataFormat, &clientDataFormat);
	if(clientChannelLayout)
		SetClientChannelLayout(*clientChannelLayout);
}

CXXCoreAudio::CAChannelLayout CXXAudioToolbox::CAExtAudioFile::ClientChannelLayout() const
{
	auto size = GetPropertyInfo(kExtAudioFileProperty_ClientChannelLayout, nullptr);
	std::unique_ptr<AudioChannelLayout, free_deleter> layout{static_cast<AudioChannelLayout *>(std::malloc(size))};
	if(!layout)
		throw std::bad_alloc();
	GetProperty(kExtAudioFileProperty_ClientChannelLayout, size, layout.get());
	return layout.get();
}

void CXXAudioToolbox::CAExtAudioFile::SetClientChannelLayout(const AudioChannelLayout& clientChannelLayout)
{
	SetProperty(kExtAudioFileProperty_ClientChannelLayout, static_cast<UInt32>(CXXCoreAudio::AudioChannelLayoutSize(&clientChannelLayout)), &clientChannelLayout);
}

AudioConverterRef CXXAudioToolbox::CAExtAudioFile::AudioConverter() const
{
	UInt32 size = sizeof(AudioConverterRef);
	AudioConverterRef converter = nullptr;
	GetProperty(kExtAudioFileProperty_AudioConverter, size, &converter);
	return converter;
}

void CXXAudioToolbox::CAExtAudioFile::SetAudioConverterProperty(AudioConverterPropertyID inPropertyID, UInt32 inPropertyDataSize, const void *inPropertyData)
{
	const auto result = AudioConverterSetProperty(AudioConverter(), inPropertyID, inPropertyDataSize, inPropertyData);
	ThrowIfAudioConverterError(result, "AudioConverterSetProperty");
	CFPropertyListRef config = nullptr;
	SetProperty(kExtAudioFileProperty_ConverterConfig, sizeof config, &config);
}

SInt64 CXXAudioToolbox::CAExtAudioFile::FrameLength() const
{
	SInt64 frameLength;
	UInt32 size = sizeof frameLength;
	GetProperty(kExtAudioFileProperty_FileLengthFrames, size, &frameLength);
	return frameLength;
}
