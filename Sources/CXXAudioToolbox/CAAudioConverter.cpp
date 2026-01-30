//
// SPDX-FileCopyrightText: 2024 Stephen F. Booth <contact@sbooth.dev>
// SPDX-License-Identifier: MIT
//
// Part of https://github.com/sbooth/CXXAudioToolbox
//

#include "audio_toolbox/CAAudioConverter.hpp"

#include "AudioToolboxErrors.hpp"

audio_toolbox::CAAudioConverter::~CAAudioConverter() noexcept { reset(); }

audio_toolbox::CAAudioConverter::CAAudioConverter(CAAudioConverter &&other) noexcept : converter_{other.release()} {}

audio_toolbox::CAAudioConverter &audio_toolbox::CAAudioConverter::operator=(CAAudioConverter &&other) noexcept {
    reset(other.release());
    return *this;
}

void audio_toolbox::CAAudioConverter::New(const AudioStreamBasicDescription &inSourceFormat,
                                            const AudioStreamBasicDescription &inDestinationFormat) {
    Dispose();
    const auto result = AudioConverterNew(&inSourceFormat, &inDestinationFormat, &converter_);
    ThrowIfAudioConverterError(result, "AudioConverterNew");
}

void audio_toolbox::CAAudioConverter::NewSpecific(const AudioStreamBasicDescription &inSourceFormat,
                                                    const AudioStreamBasicDescription &inDestinationFormat,
                                                    UInt32 inNumberClassDescriptions,
                                                    const AudioClassDescription *inClassDescriptions) {
    Dispose();
    const auto result = AudioConverterNewSpecific(&inSourceFormat, &inDestinationFormat, inNumberClassDescriptions,
                                                  inClassDescriptions, &converter_);
    ThrowIfAudioConverterError(result, "AudioConverterNewSpecific");
}

void audio_toolbox::CAAudioConverter::Dispose() {
    if (converter_) {
        const auto result = AudioConverterDispose(converter_);
        converter_ = nullptr;
        ThrowIfAudioConverterError(result, "AudioConverterDispose");
    }
}

void audio_toolbox::CAAudioConverter::Reset() {
    const auto result = AudioConverterReset(converter_);
    ThrowIfAudioConverterError(result, "AudioConverterReset");
}

void audio_toolbox::CAAudioConverter::GetPropertyInfo(AudioConverterPropertyID inPropertyID, UInt32 *outSize,
                                                        Boolean *outWritable) {
    const auto result = AudioConverterGetPropertyInfo(converter_, inPropertyID, outSize, outWritable);
    ThrowIfAudioConverterError(result, "AudioConverterGetPropertyInfo");
}

void audio_toolbox::CAAudioConverter::GetProperty(AudioConverterPropertyID inPropertyID, UInt32 &ioPropertyDataSize,
                                                    void *outPropertyData) {
    const auto result = AudioConverterGetProperty(converter_, inPropertyID, &ioPropertyDataSize, outPropertyData);
    ThrowIfAudioConverterError(result, "AudioConverterGetProperty");
}

void audio_toolbox::CAAudioConverter::SetProperty(AudioConverterPropertyID inPropertyID, UInt32 inPropertyDataSize,
                                                    const void *inPropertyData) {
    const auto result = AudioConverterSetProperty(converter_, inPropertyID, inPropertyDataSize, inPropertyData);
    ThrowIfAudioConverterError(result, "AudioConverterSetProperty");
}

void audio_toolbox::CAAudioConverter::ConvertBuffer(UInt32 inInputDataSize, const void *inInputData,
                                                      UInt32 &ioOutputDataSize, void *outOutputData) {
    const auto result =
            AudioConverterConvertBuffer(converter_, inInputDataSize, inInputData, &ioOutputDataSize, outOutputData);
    ThrowIfAudioConverterError(result, "AudioConverterConvertBuffer");
}

void audio_toolbox::CAAudioConverter::FillComplexBuffer(AudioConverterComplexInputDataProc inInputDataProc,
                                                          void *inInputDataProcUserData, UInt32 &ioOutputDataPacketSize,
                                                          AudioBufferList *outOutputData,
                                                          AudioStreamPacketDescription *outPacketDescription) {
    const auto result = AudioConverterFillComplexBuffer(converter_, inInputDataProc, inInputDataProcUserData,
                                                        &ioOutputDataPacketSize, outOutputData, outPacketDescription);
    ThrowIfAudioConverterError(result, "AudioConverterFillComplexBuffer");
}

void audio_toolbox::CAAudioConverter::ConvertComplexBuffer(UInt32 inNumberPCMFrames,
                                                             const AudioBufferList *inInputData,
                                                             AudioBufferList *outOutputData) {
    const auto result = AudioConverterConvertComplexBuffer(converter_, inNumberPCMFrames, inInputData, outOutputData);
    ThrowIfAudioConverterError(result, "AudioConverterConvertComplexBuffer");
}
