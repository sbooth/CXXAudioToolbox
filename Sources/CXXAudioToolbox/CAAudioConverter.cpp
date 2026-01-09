//
// SPDX-FileCopyrightText: 2024 Stephen F. Booth <contact@sbooth.dev>
// SPDX-License-Identifier: MIT
//
// Part of https://github.com/sbooth/CXXAudioToolbox
//

#import "CAAudioConverter.hpp"

CXXAudioToolbox::CAAudioConverter::~CAAudioConverter() noexcept
{
	reset();
}

CXXAudioToolbox::CAAudioConverter::CAAudioConverter(CAAudioConverter&& other) noexcept
: converter_{other.release()}
{}

CXXAudioToolbox::CAAudioConverter& CXXAudioToolbox::CAAudioConverter::operator=(CAAudioConverter&& other) noexcept
{
	reset(other.release());
	return *this;
}

std::expected<void, OSStatus> CXXAudioToolbox::CAAudioConverter::New(const AudioStreamBasicDescription& inSourceFormat, const AudioStreamBasicDescription& inDestinationFormat) noexcept
{
#if false
	if(const auto result = Dispose(); !result)
		return result;

	const auto result = AudioConverterNew(&inSourceFormat, &inDestinationFormat, &converter_);
	if(result != noErr)
		return std::unexpected(result);
	return {};
#endif

	const auto audio_converter_new = [&] -> std::expected<void, OSStatus> {
		const auto result = AudioConverterNew(&inSourceFormat, &inDestinationFormat, &converter_);
		if(result != noErr)
			return std::unexpected(result);
		return {};
	};

	return Dispose().and_then(audio_converter_new);
}

std::expected<void, OSStatus> CXXAudioToolbox::CAAudioConverter::NewSpecific(const AudioStreamBasicDescription& inSourceFormat, const AudioStreamBasicDescription& inDestinationFormat, UInt32 inNumberClassDescriptions, const AudioClassDescription *inClassDescriptions) noexcept
{
	const auto audio_converter_new_specific = [&] -> std::expected<void, OSStatus> {
		const auto result = AudioConverterNewSpecific(&inSourceFormat, &inDestinationFormat, inNumberClassDescriptions, inClassDescriptions, &converter_);
		if(result != noErr)
			return std::unexpected(result);
		return {};
	};

	return Dispose().and_then(audio_converter_new_specific);
}

std::expected<void, OSStatus> CXXAudioToolbox::CAAudioConverter::Dispose() noexcept
{
	if(converter_) {
		const auto result = AudioConverterDispose(converter_);
		converter_ = nullptr;
		if(result != noErr)
			return std::unexpected(result);
	}
	return {};
}

std::expected<void, OSStatus> CXXAudioToolbox::CAAudioConverter::Reset() noexcept
{
	const auto result = AudioConverterReset(converter_);
	if(result != noErr)
		return std::unexpected(result);
	return {};
}

std::expected<void, OSStatus> CXXAudioToolbox::CAAudioConverter::GetPropertyInfo(AudioConverterPropertyID inPropertyID, UInt32 *outSize, Boolean *outWritable) noexcept
{
	const auto result = AudioConverterGetPropertyInfo(converter_, inPropertyID, outSize, outWritable);
	if(result != noErr)
		return std::unexpected(result);
	return {};
}

std::expected<void, OSStatus> CXXAudioToolbox::CAAudioConverter::GetProperty(AudioConverterPropertyID inPropertyID, UInt32& ioPropertyDataSize, void *outPropertyData) noexcept
{
	const auto result = AudioConverterGetProperty(converter_, inPropertyID, &ioPropertyDataSize, outPropertyData);
	if(result != noErr)
		return std::unexpected(result);
	return {};
}

std::expected<void, OSStatus> CXXAudioToolbox::CAAudioConverter::SetProperty(AudioConverterPropertyID inPropertyID, UInt32 inPropertyDataSize, const void *inPropertyData) noexcept
{
	const auto result = AudioConverterSetProperty(converter_, inPropertyID, inPropertyDataSize, inPropertyData);
	if(result != noErr)
		return std::unexpected(result);
	return {};
}

std::expected<void, OSStatus> CXXAudioToolbox::CAAudioConverter::ConvertBuffer(UInt32 inInputDataSize, const void *inInputData, UInt32& ioOutputDataSize, void *outOutputData) noexcept
{
	const auto result = AudioConverterConvertBuffer(converter_, inInputDataSize, inInputData, &ioOutputDataSize, outOutputData);
	if(result != noErr)
		return std::unexpected(result);
	return {};
}

std::expected<void, OSStatus> CXXAudioToolbox::CAAudioConverter::FillComplexBuffer(AudioConverterComplexInputDataProc inInputDataProc, void *inInputDataProcUserData, UInt32& ioOutputDataPacketSize, AudioBufferList *outOutputData, AudioStreamPacketDescription *outPacketDescription) noexcept
{
	const auto result = AudioConverterFillComplexBuffer(converter_, inInputDataProc, inInputDataProcUserData, &ioOutputDataPacketSize, outOutputData, outPacketDescription);
	if(result != noErr)
		return std::unexpected(result);
	return {};
}

std::expected<void, OSStatus> CXXAudioToolbox::CAAudioConverter::ConvertComplexBuffer(UInt32 inNumberPCMFrames, const AudioBufferList *inInputData, AudioBufferList *outOutputData) noexcept
{
	const auto result = AudioConverterConvertComplexBuffer(converter_, inNumberPCMFrames, inInputData, outOutputData);
	if(result != noErr)
		return std::unexpected(result);
	return {};
}
