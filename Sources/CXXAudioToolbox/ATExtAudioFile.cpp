//
// Copyright © 2021-2025 Stephen F. Booth
// Part of https://github.com/sbooth/CXXAudioToolbox
// MIT license
//

#import <utility>

#import "ATExtAudioFile.hpp"
#import "ATErrors.hpp"

namespace {

struct free_deleter {
	template <typename T>
	void operator()(T * _Nonnull ptr) const {
		std::free(const_cast<std::remove_const_t<T> *>(ptr));
	}
};

}

CXXAudioToolbox::ATExtAudioFile::~ATExtAudioFile() noexcept
{
	if(extAudioFile_)
		ExtAudioFileDispose(extAudioFile_);
}

CXXAudioToolbox::ATExtAudioFile::ATExtAudioFile(ATExtAudioFile&& rhs) noexcept
: extAudioFile_{std::exchange(rhs.extAudioFile_, nullptr)}
{}

CXXAudioToolbox::ATExtAudioFile& CXXAudioToolbox::ATExtAudioFile::operator=(ATExtAudioFile&& rhs) noexcept
{
	if(this != &rhs) {
		if(extAudioFile_)
			ExtAudioFileDispose(extAudioFile_);
		extAudioFile_ = std::exchange(rhs.extAudioFile_, nullptr);
	}
	return *this;
}

void CXXAudioToolbox::ATExtAudioFile::OpenURL(CFURLRef inURL)
{
	Close();
	auto result = ExtAudioFileOpenURL(inURL, &extAudioFile_);
	ThrowIfExtAudioFileError(result, "ExtAudioFileOpenURL");
}

void CXXAudioToolbox::ATExtAudioFile::WrapAudioFileID(AudioFileID inFileID, bool inForWriting)
{
	Close();
	auto result = ExtAudioFileWrapAudioFileID(inFileID, inForWriting, &extAudioFile_);
	ThrowIfExtAudioFileError(result, "ExtAudioFileWrapAudioFileID");
}

void CXXAudioToolbox::ATExtAudioFile::CreateWithURL(CFURLRef inURL, AudioFileTypeID inFileType, const AudioStreamBasicDescription& inStreamDesc, const AudioChannelLayout * _Nullable const inChannelLayout, UInt32 inFlags)
{
	Close();
	auto result = ExtAudioFileCreateWithURL(inURL, inFileType, &inStreamDesc, inChannelLayout, inFlags, &extAudioFile_);
	ThrowIfExtAudioFileError(result, "ExtAudioFileCreateWithURL");
}

void CXXAudioToolbox::ATExtAudioFile::Close()
{
	if(extAudioFile_) {
		auto result = ExtAudioFileDispose(extAudioFile_);
		extAudioFile_ = nullptr;
		ThrowIfExtAudioFileError(result, "ExtAudioFileDispose");
	}
}

void CXXAudioToolbox::ATExtAudioFile::Read(UInt32& ioNumberFrames, AudioBufferList *ioData)
{
	auto result = ExtAudioFileRead(extAudioFile_, &ioNumberFrames, ioData);
	ThrowIfExtAudioFileError(result, "ExtAudioFileRead");
}

void CXXAudioToolbox::ATExtAudioFile::Read(CXXCoreAudio::CAAudioBuffer& buffer)
{
	buffer.PrepareForReading();
	UInt32 frameCount = buffer.FrameCapacity();
	Read(frameCount, buffer);
	buffer.SetFrameLength(frameCount);
}

#if TARGET_OS_IPHONE
OSStatus CXXAudioToolbox::ATExtAudioFile::Write(UInt32 inNumberFrames, const AudioBufferList *ioData)
#else
void CXXAudioToolbox::ATExtAudioFile::Write(UInt32 inNumberFrames, const AudioBufferList *ioData)
#endif /* TARGET_OS_IPHONE */
{
	auto result = ExtAudioFileWrite(extAudioFile_, inNumberFrames, ioData);
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

void CXXAudioToolbox::ATExtAudioFile::WriteAsync(UInt32 inNumberFrames, const AudioBufferList * _Nullable ioData)
{
	auto result = ExtAudioFileWriteAsync(extAudioFile_, inNumberFrames, ioData);
	ThrowIfExtAudioFileError(result, "ExtAudioFileWriteAsync");
}

void CXXAudioToolbox::ATExtAudioFile::Seek(SInt64 inFrameOffset)
{
	auto result = ExtAudioFileSeek(extAudioFile_, inFrameOffset);
	ThrowIfExtAudioFileError(result, "ExtAudioFileSeek");
}

SInt64 CXXAudioToolbox::ATExtAudioFile::Tell() const
{
	SInt64 pos;
	auto result = ExtAudioFileTell(extAudioFile_, &pos);
	ThrowIfExtAudioFileError(result, "ExtAudioFileTell");
	return pos;
}

UInt32 CXXAudioToolbox::ATExtAudioFile::GetPropertyInfo(ExtAudioFilePropertyID inPropertyID, Boolean * _Nullable outWritable) const
{
	UInt32 size;
	auto result = ExtAudioFileGetPropertyInfo(extAudioFile_, inPropertyID, &size, outWritable);
	ThrowIfExtAudioFileError(result, "ExtAudioFileGetPropertyInfo");
	return size;
}

void CXXAudioToolbox::ATExtAudioFile::GetProperty(ExtAudioFilePropertyID inPropertyID, UInt32& ioPropertyDataSize, void *outPropertyData) const
{
	auto result = ExtAudioFileGetProperty(extAudioFile_, inPropertyID, &ioPropertyDataSize, outPropertyData);
	ThrowIfExtAudioFileError(result, "ExtAudioFileGetProperty");
}

void CXXAudioToolbox::ATExtAudioFile::SetProperty(ExtAudioFilePropertyID inPropertyID, UInt32 inPropertyDataSize, const void *inPropertyData)
{
	auto result = ExtAudioFileSetProperty(extAudioFile_, inPropertyID, inPropertyDataSize, inPropertyData);
	ThrowIfExtAudioFileError(result, "ExtAudioFileSetProperty");
}

CXXCoreAudio::CAChannelLayout CXXAudioToolbox::ATExtAudioFile::FileChannelLayout() const
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

void CXXAudioToolbox::ATExtAudioFile::SetFileChannelLayout(const AudioChannelLayout& fileChannelLayout)
{
	SetProperty(kExtAudioFileProperty_FileChannelLayout, static_cast<UInt32>(CXXCoreAudio::AudioChannelLayoutSize(&fileChannelLayout)), &fileChannelLayout);
}

CXXCoreAudio::CAStreamDescription CXXAudioToolbox::ATExtAudioFile::FileDataFormat() const
{
	CXXCoreAudio::CAStreamDescription fileDataFormat;
	UInt32 size = sizeof(fileDataFormat);
	GetProperty(kExtAudioFileProperty_FileDataFormat, size, &fileDataFormat);
	return fileDataFormat;
}

CXXCoreAudio::CAStreamDescription CXXAudioToolbox::ATExtAudioFile::ClientDataFormat() const
{
	CXXCoreAudio::CAStreamDescription clientDataFormat;
	UInt32 size = sizeof(clientDataFormat);
	GetProperty(kExtAudioFileProperty_ClientDataFormat, size, &clientDataFormat);
	return clientDataFormat;
}

void CXXAudioToolbox::ATExtAudioFile::SetClientDataFormat(const AudioStreamBasicDescription& clientDataFormat, const AudioChannelLayout * const clientChannelLayout, UInt32 codecManufacturer)
{
	if(codecManufacturer)
		SetProperty(kExtAudioFileProperty_CodecManufacturer, sizeof(codecManufacturer), &codecManufacturer);
	SetProperty(kExtAudioFileProperty_ClientDataFormat, sizeof(clientDataFormat), &clientDataFormat);
	if(clientChannelLayout)
		SetClientChannelLayout(*clientChannelLayout);
}

CXXCoreAudio::CAChannelLayout CXXAudioToolbox::ATExtAudioFile::ClientChannelLayout() const
{
	auto size = GetPropertyInfo(kExtAudioFileProperty_ClientChannelLayout, nullptr);
	std::unique_ptr<AudioChannelLayout, free_deleter> layout{static_cast<AudioChannelLayout *>(std::malloc(size))};
	if(!layout)
		throw std::bad_alloc();
	GetProperty(kExtAudioFileProperty_ClientChannelLayout, size, layout.get());
	return layout.get();
}

void CXXAudioToolbox::ATExtAudioFile::SetClientChannelLayout(const AudioChannelLayout& clientChannelLayout)
{
	SetProperty(kExtAudioFileProperty_ClientChannelLayout, static_cast<UInt32>(CXXCoreAudio::AudioChannelLayoutSize(&clientChannelLayout)), &clientChannelLayout);
}

AudioConverterRef CXXAudioToolbox::ATExtAudioFile::AudioConverter() const
{
	UInt32 size = sizeof(AudioConverterRef);
	AudioConverterRef converter = nullptr;
	GetProperty(kExtAudioFileProperty_AudioConverter, size, &converter);
	return converter;
}

void CXXAudioToolbox::ATExtAudioFile::SetAudioConverterProperty(AudioConverterPropertyID inPropertyID, UInt32 inPropertyDataSize, const void *inPropertyData)
{
	auto result = AudioConverterSetProperty(AudioConverter(), inPropertyID, inPropertyDataSize, inPropertyData);
	ThrowIfAudioConverterError(result, "AudioConverterSetProperty");
	CFPropertyListRef config = nullptr;
	SetProperty(kExtAudioFileProperty_ConverterConfig, sizeof(config), &config);
}

SInt64 CXXAudioToolbox::ATExtAudioFile::FrameLength() const
{
	SInt64 frameLength;
	UInt32 size = sizeof(frameLength);
	GetProperty(kExtAudioFileProperty_FileLengthFrames, size, &frameLength);
	return frameLength;
}
