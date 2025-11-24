//
// Copyright © 2021-2025 Stephen F. Booth
// Part of https://github.com/sbooth/CXXAudioToolbox
// MIT license
//

#import <utility>

#import "CAExtAudioFile.hpp"
#import "AudioToolboxErrors.hpp"
#import "StringFormatting.hpp"

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
	if(extAudioFile_)
		ExtAudioFileDispose(extAudioFile_);
}

CXXAudioToolbox::CAExtAudioFile::CAExtAudioFile(CAExtAudioFile&& rhs) noexcept
: extAudioFile_{std::exchange(rhs.extAudioFile_, nullptr)}
{}

CXXAudioToolbox::CAExtAudioFile& CXXAudioToolbox::CAExtAudioFile::operator=(CAExtAudioFile&& rhs) noexcept
{
	if(this != &rhs) {
		if(extAudioFile_)
			ExtAudioFileDispose(extAudioFile_);
		extAudioFile_ = std::exchange(rhs.extAudioFile_, nullptr);
	}
	return *this;
}

void CXXAudioToolbox::CAExtAudioFile::OpenURL(CFURLRef inURL)
{
	Close();
	const auto result = ExtAudioFileOpenURL(inURL, &extAudioFile_);
	CXXAudioToolbox_ThrowIfExtAudioFileError(result, concat({
		"ExtAudioFileOpenURL(",
		string_from_cftype(CFURLGetString(inURL)),
		")",
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
}

void CXXAudioToolbox::CAExtAudioFile::WrapAudioFileID(AudioFileID inFileID, bool inForWriting)
{
	Close();
	const auto result = ExtAudioFileWrapAudioFileID(inFileID, inForWriting, &extAudioFile_);
	CXXAudioToolbox_ThrowIfExtAudioFileError(result, concat({
		"ExtAudioFileWrapAudioFileID("
		"0x", to_hex_string(reinterpret_cast<uintptr_t>(inFileID)),
		inForWriting ? "true" : "false"
		")",
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
}

void CXXAudioToolbox::CAExtAudioFile::CreateWithURL(CFURLRef inURL, AudioFileTypeID inFileType, const AudioStreamBasicDescription& inStreamDesc, const AudioChannelLayout * _Nullable const inChannelLayout, UInt32 inFlags)
{
	Close();
	const auto result = ExtAudioFileCreateWithURL(inURL, inFileType, &inStreamDesc, inChannelLayout, inFlags, &extAudioFile_);
	CXXAudioToolbox_ThrowIfExtAudioFileError(result, concat({
		"ExtAudioFileCreateWithURL(",
		string_from_cftype(CFURLGetString(inURL)),
		", ", to_fourcc_string(inFileType),
		", ", to_fourcc_string(inStreamDesc.mFormatID),
		", 0x", to_hex_string(reinterpret_cast<uintptr_t>(inChannelLayout)),
		", 0x", to_hex_string(inFlags),
		")",
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
}

void CXXAudioToolbox::CAExtAudioFile::Close()
{
	if(extAudioFile_) {
		const auto result = ExtAudioFileDispose(extAudioFile_);
		extAudioFile_ = nullptr;
		CXXAudioToolbox_ThrowIfExtAudioFileError(result, concat({
			"ExtAudioFileDispose"
			" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
		}));
	}
}

void CXXAudioToolbox::CAExtAudioFile::Read(UInt32& ioNumberFrames, AudioBufferList *ioData)
{
	const auto result = ExtAudioFileRead(extAudioFile_, &ioNumberFrames, ioData);
	CXXAudioToolbox_ThrowIfExtAudioFileError(result, concat({
		"ExtAudioFileRead(",
		to_string(ioNumberFrames),
		", 0x", to_hex_string(reinterpret_cast<uintptr_t>(ioData)),
		")",
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
}

void CXXAudioToolbox::CAExtAudioFile::Read(CXXCoreAudio::CAAudioBuffer& buffer)
{
	buffer.PrepareForReading();
	UInt32 frameCount = buffer.FrameCapacity();
	Read(frameCount, buffer);
	buffer.SetFrameLength(frameCount);
}

#if TARGET_OS_IPHONE
OSStatus CXXAudioToolbox::ATExtAudioFile::Write(UInt32 inNumberFrames, const AudioBufferList *ioData)
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
			CXXAudioToolbox_ThrowIfExtAudioFileError(result, concat({
				"ExtAudioFileWrite(",
				to_string(inNumberFrames),
				", 0x", to_hex_string(reinterpret_cast<uintptr_t>(ioData)),
				")",
				" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
			}));
			break;
	}
	return result;
#else
	CXXAudioToolbox_ThrowIfExtAudioFileError(result, concat({
		"ExtAudioFileWrite(",
		to_string(inNumberFrames),
		", 0x", to_hex_string(reinterpret_cast<uintptr_t>(ioData)),
		")",
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
#endif /* TARGET_OS_IPHONE */
}

void CXXAudioToolbox::CAExtAudioFile::WriteAsync(UInt32 inNumberFrames, const AudioBufferList * _Nullable ioData)
{
	const auto result = ExtAudioFileWriteAsync(extAudioFile_, inNumberFrames, ioData);
	CXXAudioToolbox_ThrowIfExtAudioFileError(result, concat({
		"ExtAudioFileWriteAsync(",
		to_string(inNumberFrames),
		", 0x", to_hex_string(reinterpret_cast<uintptr_t>(ioData)),
		")",
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
}

void CXXAudioToolbox::CAExtAudioFile::Seek(SInt64 inFrameOffset)
{
	const auto result = ExtAudioFileSeek(extAudioFile_, inFrameOffset);
	CXXAudioToolbox_ThrowIfExtAudioFileError(result, concat({
		"ExtAudioFileSeek(",
		to_string(inFrameOffset),
		")",
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
}

SInt64 CXXAudioToolbox::CAExtAudioFile::Tell() const
{
	SInt64 pos;
	const auto result = ExtAudioFileTell(extAudioFile_, &pos);
	CXXAudioToolbox_ThrowIfExtAudioFileError(result, concat({
		"ExtAudioFileTell"
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
	return pos;
}

UInt32 CXXAudioToolbox::CAExtAudioFile::GetPropertyInfo(ExtAudioFilePropertyID inPropertyID, Boolean * _Nullable outWritable) const
{
	UInt32 size;
	const auto result = ExtAudioFileGetPropertyInfo(extAudioFile_, inPropertyID, &size, outWritable);
	CXXAudioToolbox_ThrowIfExtAudioFileError(result, concat({
		"ExtAudioFileGetPropertyInfo(",
		to_fourcc_string(inPropertyID),
		", 0x", to_hex_string(reinterpret_cast<uintptr_t>(outWritable)),
		")",
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
	return size;
}

void CXXAudioToolbox::CAExtAudioFile::GetProperty(ExtAudioFilePropertyID inPropertyID, UInt32& ioPropertyDataSize, void *outPropertyData) const
{
	const auto result = ExtAudioFileGetProperty(extAudioFile_, inPropertyID, &ioPropertyDataSize, outPropertyData);
	CXXAudioToolbox_ThrowIfExtAudioFileError(result, concat({
		"ExtAudioFileGetProperty(",
		to_fourcc_string(inPropertyID),
		", ", to_string(ioPropertyDataSize),
		", 0x", to_hex_string(reinterpret_cast<uintptr_t>(outPropertyData)),
		")",
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
}

void CXXAudioToolbox::CAExtAudioFile::SetProperty(ExtAudioFilePropertyID inPropertyID, UInt32 inPropertyDataSize, const void *inPropertyData)
{
	const auto result = ExtAudioFileSetProperty(extAudioFile_, inPropertyID, inPropertyDataSize, inPropertyData);
	CXXAudioToolbox_ThrowIfExtAudioFileError(result, concat({
		"ExtAudioFileSetProperty(",
		to_fourcc_string(inPropertyID),
		", ", to_string(inPropertyDataSize),
		", 0x", to_hex_string(reinterpret_cast<uintptr_t>(inPropertyData)),
		")",
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
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
	UInt32 size = sizeof(fileDataFormat);
	GetProperty(kExtAudioFileProperty_FileDataFormat, size, &fileDataFormat);
	return fileDataFormat;
}

CXXCoreAudio::CAStreamDescription CXXAudioToolbox::CAExtAudioFile::ClientDataFormat() const
{
	CXXCoreAudio::CAStreamDescription clientDataFormat;
	UInt32 size = sizeof(clientDataFormat);
	GetProperty(kExtAudioFileProperty_ClientDataFormat, size, &clientDataFormat);
	return clientDataFormat;
}

void CXXAudioToolbox::CAExtAudioFile::SetClientDataFormat(const AudioStreamBasicDescription& clientDataFormat, const AudioChannelLayout * const clientChannelLayout, UInt32 codecManufacturer)
{
	if(codecManufacturer)
		SetProperty(kExtAudioFileProperty_CodecManufacturer, sizeof(codecManufacturer), &codecManufacturer);
	SetProperty(kExtAudioFileProperty_ClientDataFormat, sizeof(clientDataFormat), &clientDataFormat);
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
	CXXAudioToolbox_ThrowIfAudioConverterError(result, concat({
		"AudioConverterSetProperty(",
		to_fourcc_string(inPropertyID),
		", ", to_string(inPropertyDataSize),
		", 0x", to_hex_string(reinterpret_cast<uintptr_t>(inPropertyData)),
		")",
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
	CFPropertyListRef config = nullptr;
	SetProperty(kExtAudioFileProperty_ConverterConfig, sizeof(config), &config);
}

SInt64 CXXAudioToolbox::CAExtAudioFile::FrameLength() const
{
	SInt64 frameLength;
	UInt32 size = sizeof(frameLength);
	GetProperty(kExtAudioFileProperty_FileLengthFrames, size, &frameLength);
	return frameLength;
}
