//
// Copyright © 2024-2025 Stephen F. Booth
// Part of https://github.com/sbooth/CXXAudioToolbox
// MIT license
//

#import <utility>

#import "CAAudioConverter.hpp"
#import "AudioToolboxErrors.hpp"

CXXAudioToolbox::CAAudioConverter::~CAAudioConverter() noexcept
{
	if(converter_)
		AudioConverterDispose(converter_);
}

CXXAudioToolbox::CAAudioConverter::CAAudioConverter(CAAudioConverter&& rhs) noexcept
: converter_{std::exchange(rhs.converter_, nullptr)}
{}

CXXAudioToolbox::CAAudioConverter& CXXAudioToolbox::CAAudioConverter::operator=(CAAudioConverter&& rhs) noexcept
{
	if(this != &rhs) {
		if(converter_)
			AudioConverterDispose(converter_);
		converter_ = std::exchange(rhs.converter_, nullptr);
	}
	return *this;
}

void CXXAudioToolbox::CAAudioConverter::New(const AudioStreamBasicDescription& inSourceFormat, const AudioStreamBasicDescription& inDestinationFormat)
{
	Dispose();
	auto result = AudioConverterNew(&inSourceFormat, &inDestinationFormat, &converter_);
	ThrowIfAudioConverterError(result, "AudioConverterNew");
}

void CXXAudioToolbox::CAAudioConverter::NewSpecific(const AudioStreamBasicDescription& inSourceFormat, const AudioStreamBasicDescription& inDestinationFormat, UInt32 inNumberClassDescriptions, const AudioClassDescription *inClassDescriptions)
{
	Dispose();
	auto result = AudioConverterNewSpecific(&inSourceFormat, &inDestinationFormat, inNumberClassDescriptions, inClassDescriptions, &converter_);
	ThrowIfAudioConverterError(result, "AudioConverterNewSpecific");
}

void CXXAudioToolbox::CAAudioConverter::Dispose()
{
	if(converter_) {
		auto result = AudioConverterDispose(converter_);
		converter_ = nullptr;
		ThrowIfAudioConverterError(result, "AudioConverterDispose");
	}
}

void CXXAudioToolbox::CAAudioConverter::Reset()
{
	auto result = AudioConverterReset(converter_);
	ThrowIfAudioConverterError(result, "AudioConverterReset");
}

void CXXAudioToolbox::CAAudioConverter::GetPropertyInfo(AudioConverterPropertyID inPropertyID, UInt32 *outSize, Boolean *outWritable)
{
	auto result = AudioConverterGetPropertyInfo(converter_, inPropertyID, outSize, outWritable);
	ThrowIfAudioConverterError(result, "AudioConverterGetPropertyInfo");
}

void CXXAudioToolbox::CAAudioConverter::GetProperty(AudioConverterPropertyID inPropertyID, UInt32& ioPropertyDataSize, void *outPropertyData)
{
	auto result = AudioConverterGetProperty(converter_, inPropertyID, &ioPropertyDataSize, outPropertyData);
	ThrowIfAudioConverterError(result, "AudioConverterGetProperty");
}

void CXXAudioToolbox::CAAudioConverter::SetProperty(AudioConverterPropertyID inPropertyID, UInt32 inPropertyDataSize, const void *inPropertyData)
{
	auto result = AudioConverterSetProperty(converter_, inPropertyID, inPropertyDataSize, inPropertyData);
	ThrowIfAudioConverterError(result, "AudioConverterSetProperty");
}

void CXXAudioToolbox::CAAudioConverter::ConvertBuffer(UInt32 inInputDataSize, const void *inInputData, UInt32& ioOutputDataSize, void *outOutputData)
{
	auto result = AudioConverterConvertBuffer(converter_, inInputDataSize, inInputData, &ioOutputDataSize, outOutputData);
	ThrowIfAudioConverterError(result, "AudioConverterConvertBuffer");
}

void CXXAudioToolbox::CAAudioConverter::FillComplexBuffer(AudioConverterComplexInputDataProc inInputDataProc, void *inInputDataProcUserData, UInt32& ioOutputDataPacketSize, AudioBufferList *outOutputData, AudioStreamPacketDescription *outPacketDescription)
{
	auto result = AudioConverterFillComplexBuffer(converter_, inInputDataProc, inInputDataProcUserData, &ioOutputDataPacketSize, outOutputData, outPacketDescription);
	ThrowIfAudioConverterError(result, "AudioConverterFillComplexBuffer");
}

void CXXAudioToolbox::CAAudioConverter::ConvertComplexBuffer(UInt32 inNumberPCMFrames, const AudioBufferList *inInputData, AudioBufferList *outOutputData)
{
	auto result = AudioConverterConvertComplexBuffer(converter_, inNumberPCMFrames, inInputData, outOutputData);
	ThrowIfAudioConverterError(result, "AudioConverterConvertComplexBuffer");
}
