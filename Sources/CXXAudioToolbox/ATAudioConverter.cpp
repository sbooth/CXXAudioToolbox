//
// Copyright © 2024-2025 Stephen F. Booth
// Part of https://github.com/sbooth/CXXAudioToolbox
// MIT license
//

#import <exception>
#import <system_error>
#import <utility>

#import "ATAudioConverter.hpp"

namespace {

/// A std::error_category for return values from the AudioConverter API.
class AudioConverterErrorCategory : public std::error_category {
public:
	virtual const char * name() const noexcept override final { return "AudioConverter"; }
	virtual std::string message(int condition) const override final
	{
		switch(static_cast<OSStatus>(condition)) {
				// CoreAudioBaseTypes.h
			case kAudio_NoError: 										return "The function call completed successfully";
			case kAudio_UnimplementedError: 							return "Unimplemented core routine";
			case kAudio_FileNotFoundError: 								return "File not found";
			case kAudio_FilePermissionError: 							return "File cannot be opened due to either file, directory, or sandbox permissions";
			case kAudio_TooManyFilesOpenError: 							return "File cannot be opened because too many files are already open";
			case kAudio_BadFilePathError: 								return "File cannot be opened because the specified path is malformed";
			case kAudio_ParamError: 									return "Error in user parameter list";
			case kAudio_MemFullError: 									return "Not enough room in heap zone";
				// AudioConverter.h
			case kAudioConverterErr_FormatNotSupported: 				return "kAudioConverterErr_FormatNotSupported or kAudioFileUnsupportedDataFormatError";
			case kAudioConverterErr_OperationNotSupported: 				return "kAudioConverterErr_OperationNotSupported";
			case kAudioConverterErr_PropertyNotSupported: 				return "kAudioConverterErr_PropertyNotSupported";
			case kAudioConverterErr_InvalidInputSize: 					return "kAudioConverterErr_InvalidInputSize";
			case kAudioConverterErr_InvalidOutputSize: 					return "kAudioConverterErr_InvalidOutputSize";
			case kAudioConverterErr_UnspecifiedError: 					return "kAudioConverterErr_UnspecifiedError";
			case kAudioConverterErr_BadPropertySizeError: 				return "kAudioConverterErr_BadPropertySizeError";
			case kAudioConverterErr_RequiresPacketDescriptionsError: 	return "kAudioConverterErr_RequiresPacketDescriptionsError";
			case kAudioConverterErr_InputSampleRateOutOfRange: 			return "kAudioConverterErr_InputSampleRateOutOfRange";
			case kAudioConverterErr_OutputSampleRateOutOfRange: 		return "kAudioConverterErr_OutputSampleRateOutOfRange";
#if TARGET_OS_IPHONE
			case kAudioConverterErr_HardwareInUse: 						return "kAudioConverterErr_HardwareInUse";
			case kAudioConverterErr_NoHardwarePermission: 				return "kAudioConverterErr_NoHardwarePermission";
#endif /* TARGET_OS_IPHONE */
			default:													return "Unknown Audio Converter error";
		}
	}
};

/// Global instance of audio converter error category.
const AudioConverterErrorCategory audioConverterErrorCategory_;

/// Throws a std::system_error in the AudioConverterErrorCategory if result != kAudio_NoError.
/// @param result An OSStatus result code.
/// @param operation An optional string describing the operation that produced result.
/// @throw std::system_error in the AudioConverterErrorCategory.
inline void ThrowIfAudioConverterError(OSStatus result, const char * const operation = nullptr)
{
	if(__builtin_expect(result != kAudio_NoError, false))
		throw std::system_error(result, audioConverterErrorCategory_, operation);
}

} /* namespace */

CXXAudioToolbox::ATAudioConverter::~ATAudioConverter() noexcept
{
	if(converter_)
		AudioConverterDispose(converter_);
}

CXXAudioToolbox::ATAudioConverter::ATAudioConverter(ATAudioConverter&& rhs) noexcept
: converter_{std::exchange(rhs.converter_, nullptr)}
{}

CXXAudioToolbox::ATAudioConverter& CXXAudioToolbox::ATAudioConverter::operator=(ATAudioConverter&& rhs) noexcept
{
	if(this != &rhs) {
		if(converter_)
			AudioConverterDispose(converter_);
		converter_ = std::exchange(rhs.converter_, nullptr);
	}
	return *this;
}

void CXXAudioToolbox::ATAudioConverter::New(const AudioStreamBasicDescription& inSourceFormat, const AudioStreamBasicDescription& inDestinationFormat)
{
	Dispose();
	auto result = AudioConverterNew(&inSourceFormat, &inDestinationFormat, &converter_);
	ThrowIfAudioConverterError(result, "AudioConverterNew");
}

void CXXAudioToolbox::ATAudioConverter::NewSpecific(const AudioStreamBasicDescription& inSourceFormat, const AudioStreamBasicDescription& inDestinationFormat, UInt32 inNumberClassDescriptions, const AudioClassDescription *inClassDescriptions)
{
	Dispose();
	auto result = AudioConverterNewSpecific(&inSourceFormat, &inDestinationFormat, inNumberClassDescriptions, inClassDescriptions, &converter_);
	ThrowIfAudioConverterError(result, "AudioConverterNewSpecific");
}

void CXXAudioToolbox::ATAudioConverter::Dispose()
{
	if(converter_) {
		auto result = AudioConverterDispose(converter_);
		converter_ = nullptr;
		ThrowIfAudioConverterError(result, "AudioConverterDispose");
	}
}

void CXXAudioToolbox::ATAudioConverter::Reset()
{
	auto result = AudioConverterReset(converter_);
	ThrowIfAudioConverterError(result, "AudioConverterReset");
}

void CXXAudioToolbox::ATAudioConverter::GetPropertyInfo(AudioConverterPropertyID inPropertyID, UInt32 * _Nullable outSize, Boolean * _Nullable outWritable)
{
	auto result = AudioConverterGetPropertyInfo(converter_, inPropertyID, outSize, outWritable);
	ThrowIfAudioConverterError(result, "AudioConverterGetPropertyInfo");
}

void CXXAudioToolbox::ATAudioConverter::GetProperty(AudioConverterPropertyID inPropertyID, UInt32& ioPropertyDataSize, void *outPropertyData)
{
	auto result = AudioConverterGetProperty(converter_, inPropertyID, &ioPropertyDataSize, outPropertyData);
	ThrowIfAudioConverterError(result, "AudioConverterGetProperty");
}

void CXXAudioToolbox::ATAudioConverter::SetProperty(AudioConverterPropertyID inPropertyID, UInt32 inPropertyDataSize, const void *inPropertyData)
{
	auto result = AudioConverterSetProperty(converter_, inPropertyID, inPropertyDataSize, inPropertyData);
	ThrowIfAudioConverterError(result, "AudioConverterSetProperty");
}

void CXXAudioToolbox::ATAudioConverter::ConvertBuffer(UInt32 inInputDataSize, const void *inInputData, UInt32& ioOutputDataSize, void *outOutputData)
{
	auto result = AudioConverterConvertBuffer(converter_, inInputDataSize, inInputData, &ioOutputDataSize, outOutputData);
	ThrowIfAudioConverterError(result, "AudioConverterConvertBuffer");
}

void CXXAudioToolbox::ATAudioConverter::FillComplexBuffer(AudioConverterComplexInputDataProc inInputDataProc, void * _Nullable inInputDataProcUserData, UInt32& ioOutputDataPacketSize, AudioBufferList *outOutputData, AudioStreamPacketDescription * _Nullable outPacketDescription)
{
	auto result = AudioConverterFillComplexBuffer(converter_, inInputDataProc, inInputDataProcUserData, &ioOutputDataPacketSize, outOutputData, outPacketDescription);
	ThrowIfAudioConverterError(result, "AudioConverterFillComplexBuffer");
}

void CXXAudioToolbox::ATAudioConverter::ConvertComplexBuffer(UInt32 inNumberPCMFrames, const AudioBufferList *inInputData, AudioBufferList *outOutputData)
{
	auto result = AudioConverterConvertComplexBuffer(converter_, inNumberPCMFrames, inInputData, outOutputData);
	ThrowIfAudioConverterError(result, "AudioConverterConvertComplexBuffer");
}
