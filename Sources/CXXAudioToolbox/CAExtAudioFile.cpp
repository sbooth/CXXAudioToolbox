//
// SPDX-FileCopyrightText: 2021 Stephen F. Booth <contact@sbooth.dev>
// SPDX-License-Identifier: MIT
//
// Part of https://github.com/sbooth/CXXAudioToolbox
//

#include "audio_toolbox/CAExtAudioFile.hpp"

#include "AudioToolboxErrors.hpp"

namespace {

struct free_deleter {
    template <typename T> void operator()(T *_Nonnull ptr) const {
        std::free(const_cast<std::remove_const_t<T> *>(ptr));
    }
};

} /* namespace */

audio_toolbox::CAExtAudioFile::~CAExtAudioFile() noexcept { reset(); }

audio_toolbox::CAExtAudioFile::CAExtAudioFile(CAExtAudioFile &&other) noexcept : extAudioFile_{other.release()} {}

audio_toolbox::CAExtAudioFile &audio_toolbox::CAExtAudioFile::operator=(CAExtAudioFile &&other) noexcept {
    reset(other.release());
    return *this;
}

void audio_toolbox::CAExtAudioFile::OpenURL(CFURLRef inURL) {
    Dispose();
    const auto result = ExtAudioFileOpenURL(inURL, &extAudioFile_);
    ThrowIfExtAudioFileError(result, "ExtAudioFileOpenURL");
}

void audio_toolbox::CAExtAudioFile::WrapAudioFileID(AudioFileID inFileID, bool inForWriting) {
    Dispose();
    const auto result = ExtAudioFileWrapAudioFileID(inFileID, inForWriting, &extAudioFile_);
    ThrowIfExtAudioFileError(result, "ExtAudioFileWrapAudioFileID");
}

void audio_toolbox::CAExtAudioFile::CreateWithURL(CFURLRef inURL, AudioFileTypeID inFileType,
                                                  const AudioStreamBasicDescription &inStreamDesc,
                                                  const AudioChannelLayout *_Nullable const inChannelLayout,
                                                  UInt32 inFlags) {
    Dispose();
    const auto result =
            ExtAudioFileCreateWithURL(inURL, inFileType, &inStreamDesc, inChannelLayout, inFlags, &extAudioFile_);
    ThrowIfExtAudioFileError(result, "ExtAudioFileCreateWithURL");
}

void audio_toolbox::CAExtAudioFile::Dispose() {
    if (extAudioFile_) {
        const auto result = ExtAudioFileDispose(extAudioFile_);
        extAudioFile_ = nullptr;
        ThrowIfExtAudioFileError(result, "ExtAudioFileDispose");
    }
}

void audio_toolbox::CAExtAudioFile::Read(UInt32 &ioNumberFrames, AudioBufferList *ioData) {
    const auto result = ExtAudioFileRead(extAudioFile_, &ioNumberFrames, ioData);
    ThrowIfExtAudioFileError(result, "ExtAudioFileRead");
}

void audio_toolbox::CAExtAudioFile::Read(core_audio::BufferList &buffer) {
    buffer.prepareForReading();
    UInt32 frameCount = buffer.frameCapacity();
    Read(frameCount, buffer);
    buffer.setFrameLength(frameCount);
}

#if TARGET_OS_IPHONE
OSStatus audio_toolbox::CAExtAudioFile::Write(UInt32 inNumberFrames, const AudioBufferList *ioData)
#else
void audio_toolbox::CAExtAudioFile::Write(UInt32 inNumberFrames, const AudioBufferList *ioData)
#endif /* TARGET_OS_IPHONE */
{
    const auto result = ExtAudioFileWrite(extAudioFile_, inNumberFrames, ioData);
#if TARGET_OS_IPHONE
    switch (result) {
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

void audio_toolbox::CAExtAudioFile::WriteAsync(UInt32 inNumberFrames, const AudioBufferList *_Nullable ioData) {
    const auto result = ExtAudioFileWriteAsync(extAudioFile_, inNumberFrames, ioData);
    ThrowIfExtAudioFileError(result, "ExtAudioFileWriteAsync");
}

void audio_toolbox::CAExtAudioFile::Seek(SInt64 inFrameOffset) {
    const auto result = ExtAudioFileSeek(extAudioFile_, inFrameOffset);
    ThrowIfExtAudioFileError(result, "ExtAudioFileSeek");
}

SInt64 audio_toolbox::CAExtAudioFile::Tell() const {
    SInt64 pos;
    const auto result = ExtAudioFileTell(extAudioFile_, &pos);
    ThrowIfExtAudioFileError(result, "ExtAudioFileTell");
    return pos;
}

void audio_toolbox::CAExtAudioFile::GetPropertyInfo(ExtAudioFilePropertyID inPropertyID, UInt32 *_Nullable outSize,
                                                    Boolean *_Nullable outWritable) const {
    const auto result = ExtAudioFileGetPropertyInfo(extAudioFile_, inPropertyID, outSize, outWritable);
    ThrowIfExtAudioFileError(result, "ExtAudioFileGetPropertyInfo");
}

void audio_toolbox::CAExtAudioFile::GetProperty(ExtAudioFilePropertyID inPropertyID, UInt32 &ioPropertyDataSize,
                                                void *outPropertyData) const {
    const auto result = ExtAudioFileGetProperty(extAudioFile_, inPropertyID, &ioPropertyDataSize, outPropertyData);
    ThrowIfExtAudioFileError(result, "ExtAudioFileGetProperty");
}

void audio_toolbox::CAExtAudioFile::SetProperty(ExtAudioFilePropertyID inPropertyID, UInt32 inPropertyDataSize,
                                                const void *inPropertyData) {
    const auto result = ExtAudioFileSetProperty(extAudioFile_, inPropertyID, inPropertyDataSize, inPropertyData);
    ThrowIfExtAudioFileError(result, "ExtAudioFileSetProperty");
}

core_audio::ChannelLayout audio_toolbox::CAExtAudioFile::FileChannelLayout() const {
    UInt32 size;
    GetPropertyInfo(kExtAudioFileProperty_FileChannelLayout, &size, nullptr);
    std::unique_ptr<AudioChannelLayout, free_deleter> layout{static_cast<AudioChannelLayout *>(std::malloc(size))};
    if (!layout) {
        throw std::bad_alloc();
    }
    GetProperty(kExtAudioFileProperty_FileChannelLayout, size, layout.get());
    core_audio::ChannelLayout channelLayout{};
    channelLayout.reset(layout.release());
    return channelLayout;
}

void audio_toolbox::CAExtAudioFile::SetFileChannelLayout(const AudioChannelLayout &fileChannelLayout) {
    SetProperty(kExtAudioFileProperty_FileChannelLayout,
                static_cast<UInt32>(core_audio::audioChannelLayoutSize(&fileChannelLayout)), &fileChannelLayout);
}

core_audio::StreamDescription audio_toolbox::CAExtAudioFile::FileDataFormat() const {
    core_audio::StreamDescription fileDataFormat;
    UInt32 size = sizeof fileDataFormat;
    GetProperty(kExtAudioFileProperty_FileDataFormat, size, &fileDataFormat);
    return fileDataFormat;
}

core_audio::StreamDescription audio_toolbox::CAExtAudioFile::ClientDataFormat() const {
    core_audio::StreamDescription clientDataFormat;
    UInt32 size = sizeof clientDataFormat;
    GetProperty(kExtAudioFileProperty_ClientDataFormat, size, &clientDataFormat);
    return clientDataFormat;
}

void audio_toolbox::CAExtAudioFile::SetClientDataFormat(const AudioStreamBasicDescription &clientDataFormat,
                                                        const AudioChannelLayout *const clientChannelLayout,
                                                        UInt32 codecManufacturer) {
    if (codecManufacturer) {
        SetProperty(kExtAudioFileProperty_CodecManufacturer, sizeof codecManufacturer, &codecManufacturer);
    }
    SetProperty(kExtAudioFileProperty_ClientDataFormat, sizeof clientDataFormat, &clientDataFormat);
    if (clientChannelLayout) {
        SetClientChannelLayout(*clientChannelLayout);
    }
}

core_audio::ChannelLayout audio_toolbox::CAExtAudioFile::ClientChannelLayout() const {
    UInt32 size;
    GetPropertyInfo(kExtAudioFileProperty_ClientChannelLayout, &size, nullptr);
    std::unique_ptr<AudioChannelLayout, free_deleter> layout{static_cast<AudioChannelLayout *>(std::malloc(size))};
    if (!layout) {
        throw std::bad_alloc();
    }
    GetProperty(kExtAudioFileProperty_ClientChannelLayout, size, layout.get());
    return layout.get();
}

void audio_toolbox::CAExtAudioFile::SetClientChannelLayout(const AudioChannelLayout &clientChannelLayout) {
    SetProperty(kExtAudioFileProperty_ClientChannelLayout,
                static_cast<UInt32>(core_audio::audioChannelLayoutSize(&clientChannelLayout)), &clientChannelLayout);
}

AudioConverterRef audio_toolbox::CAExtAudioFile::AudioConverter() const {
    UInt32 size = sizeof(AudioConverterRef);
    AudioConverterRef converter = nullptr;
    GetProperty(kExtAudioFileProperty_AudioConverter, size, &converter);
    return converter;
}

void audio_toolbox::CAExtAudioFile::SetAudioConverterProperty(AudioConverterPropertyID inPropertyID,
                                                              UInt32 inPropertyDataSize, const void *inPropertyData) {
    const auto result = AudioConverterSetProperty(AudioConverter(), inPropertyID, inPropertyDataSize, inPropertyData);
    ThrowIfAudioConverterError(result, "AudioConverterSetProperty");
    CFPropertyListRef config = nullptr;
    SetProperty(kExtAudioFileProperty_ConverterConfig, sizeof config, &config);
}

SInt64 audio_toolbox::CAExtAudioFile::FrameLength() const {
    SInt64 frameLength;
    UInt32 size = sizeof frameLength;
    GetProperty(kExtAudioFileProperty_FileLengthFrames, size, &frameLength);
    return frameLength;
}
