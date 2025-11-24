//
// Copyright © 2024-2025 Stephen F. Booth
// Part of https://github.com/sbooth/CXXAudioToolbox
// MIT license
//

#import <utility>

#import "CAAudioConverter.hpp"
#import "AudioToolboxErrors.hpp"
#import "StringFormatting.hpp"

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
	const auto result = AudioConverterNew(&inSourceFormat, &inDestinationFormat, &converter_);
	CXXAudioToolbox_ThrowIfAudioConverterError(result, concat({
		"AudioConverterNew(",
		to_fourcc_string(inSourceFormat.mFormatID),
		", ", to_fourcc_string(inDestinationFormat.mFormatID),
		")",
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
}

void CXXAudioToolbox::CAAudioConverter::NewSpecific(const AudioStreamBasicDescription& inSourceFormat, const AudioStreamBasicDescription& inDestinationFormat, UInt32 inNumberClassDescriptions, const AudioClassDescription *inClassDescriptions)
{
	Dispose();
	const auto result = AudioConverterNewSpecific(&inSourceFormat, &inDestinationFormat, inNumberClassDescriptions, inClassDescriptions, &converter_);
	CXXAudioToolbox_ThrowIfAudioConverterError(result, concat({
		"AudioConverterNewSpecific(",
		to_fourcc_string(inSourceFormat.mFormatID),
		", ", to_fourcc_string(inDestinationFormat.mFormatID),
		", ", to_string(inNumberClassDescriptions),
		", 0x", to_hex_string(reinterpret_cast<uintptr_t>(inClassDescriptions)),
		")",
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
}

void CXXAudioToolbox::CAAudioConverter::Dispose()
{
	if(converter_) {
		const auto result = AudioConverterDispose(converter_);
		converter_ = nullptr;
		CXXAudioToolbox_ThrowIfAudioConverterError(result, concat({
			"AudioConverterDispose"
			" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
		}));
	}
}

void CXXAudioToolbox::CAAudioConverter::Reset()
{
	const auto result = AudioConverterReset(converter_);
	CXXAudioToolbox_ThrowIfAudioConverterError(result, concat({
		"AudioConverterReset"
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
}

UInt32 CXXAudioToolbox::CAAudioConverter::GetPropertyInfo(AudioConverterPropertyID inPropertyID, Boolean *outWritable)
{
	UInt32 size;
	const auto result = AudioConverterGetPropertyInfo(converter_, inPropertyID, &size, outWritable);
	CXXAudioToolbox_ThrowIfAudioConverterError(result, concat({
		"AudioConverterGetPropertyInfo(",
		to_fourcc_string(inPropertyID),
		", 0x", to_hex_string(reinterpret_cast<uintptr_t>(outWritable)),
		")",
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
	return size;
}

void CXXAudioToolbox::CAAudioConverter::GetProperty(AudioConverterPropertyID inPropertyID, UInt32& ioPropertyDataSize, void *outPropertyData)
{
	const auto result = AudioConverterGetProperty(converter_, inPropertyID, &ioPropertyDataSize, outPropertyData);
	CXXAudioToolbox_ThrowIfAudioConverterError(result, concat({
		"AudioConverterGetProperty(",
		to_fourcc_string(inPropertyID),
		", ", to_string(ioPropertyDataSize),
		", 0x", to_hex_string(reinterpret_cast<uintptr_t>(outPropertyData)),
		")",
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
}

void CXXAudioToolbox::CAAudioConverter::SetProperty(AudioConverterPropertyID inPropertyID, UInt32 inPropertyDataSize, const void *inPropertyData)
{
	const auto result = AudioConverterSetProperty(converter_, inPropertyID, inPropertyDataSize, inPropertyData);
	CXXAudioToolbox_ThrowIfAudioConverterError(result, concat({
		"AudioConverterSetProperty(",
		to_fourcc_string(inPropertyID),
		", ", to_string(inPropertyDataSize),
		", 0x", to_hex_string(reinterpret_cast<uintptr_t>(inPropertyData)),
		")",
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
}

void CXXAudioToolbox::CAAudioConverter::ConvertBuffer(UInt32 inInputDataSize, const void *inInputData, UInt32& ioOutputDataSize, void *outOutputData)
{
	const auto result = AudioConverterConvertBuffer(converter_, inInputDataSize, inInputData, &ioOutputDataSize, outOutputData);
	CXXAudioToolbox_ThrowIfAudioConverterError(result, concat({
		"AudioConverterConvertBuffer(",
		to_string(inInputDataSize),
		", 0x", to_hex_string(reinterpret_cast<uintptr_t>(inInputData)),
		to_string(ioOutputDataSize),
		", 0x", to_hex_string(reinterpret_cast<uintptr_t>(outOutputData)),
		")",
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
}

void CXXAudioToolbox::CAAudioConverter::FillComplexBuffer(AudioConverterComplexInputDataProc inInputDataProc, void *inInputDataProcUserData, UInt32& ioOutputDataPacketSize, AudioBufferList *outOutputData, AudioStreamPacketDescription *outPacketDescription)
{
	const auto result = AudioConverterFillComplexBuffer(converter_, inInputDataProc, inInputDataProcUserData, &ioOutputDataPacketSize, outOutputData, outPacketDescription);
	CXXAudioToolbox_ThrowIfAudioConverterError(result, concat({
		"AudioConverterFillComplexBuffer("
		"0x", to_hex_string(reinterpret_cast<uintptr_t>(inInputDataProc)),
		", 0x", to_hex_string(reinterpret_cast<uintptr_t>(inInputDataProcUserData)),
		", ", to_string(ioOutputDataPacketSize),
		", 0x", to_hex_string(reinterpret_cast<uintptr_t>(outOutputData)),
		", 0x", to_hex_string(reinterpret_cast<uintptr_t>(outPacketDescription)),
		")",
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
}

void CXXAudioToolbox::CAAudioConverter::ConvertComplexBuffer(UInt32 inNumberPCMFrames, const AudioBufferList *inInputData, AudioBufferList *outOutputData)
{
	const auto result = AudioConverterConvertComplexBuffer(converter_, inNumberPCMFrames, inInputData, outOutputData);
	CXXAudioToolbox_ThrowIfAudioConverterError(result, concat({
		"AudioConverterConvertComplexBuffer(",
		to_string(inNumberPCMFrames),
		", 0x", to_hex_string(reinterpret_cast<uintptr_t>(inInputData)),
		", 0x", to_hex_string(reinterpret_cast<uintptr_t>(outOutputData)),
		")",
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
}
