//
// Copyright © 2021-2025 Stephen F. Booth
// Part of https://github.com/sbooth/CXXAudioToolbox
// MIT license
//

#import <exception>
#import <system_error>

#import "ATAudioFormat.hpp"

namespace {

/// A std::error_category for return values from the AudioFormat API.
class AudioFormatErrorCategory : public std::error_category {
public:
	virtual const char * name() const noexcept override final { return "AudioFormat"; }
	virtual std::string message(int condition) const override final
	{
		switch(static_cast<OSStatus>(condition)) {
				// CoreAudioBaseTypes.h
			case kAudio_NoError: 							return "The function call completed successfully";
			case kAudio_UnimplementedError: 				return "Unimplemented core routine";
			case kAudio_FileNotFoundError: 					return "File not found";
			case kAudio_FilePermissionError: 				return "File cannot be opened due to either file, directory, or sandbox permissions";
			case kAudio_TooManyFilesOpenError: 				return "File cannot be opened because too many files are already open";
			case kAudio_BadFilePathError: 					return "File cannot be opened because the specified path is malformed";
			case kAudio_ParamError: 						return "Error in user parameter list";
			case kAudio_MemFullError: 						return "Not enough room in heap zone";
				// AudioFormat.h
			case kAudioFormatUnspecifiedError: 				return "kAudioFormatUnspecifiedError";
			case kAudioFormatUnsupportedPropertyError: 		return "kAudioFormatUnsupportedPropertyError";
			case kAudioFormatBadPropertySizeError: 			return "kAudioFormatBadPropertySizeError";
			case kAudioFormatBadSpecifierSizeError: 		return "kAudioFormatBadSpecifierSizeError";
			case kAudioFormatUnsupportedDataFormatError: 	return "kAudioFormatUnsupportedDataFormatError";
			case kAudioFormatUnknownFormatError: 			return "kAudioFormatUnknownFormatError";
			default:										return "Unknown AudioFormat error";
		}
	}
};

/// Global instance of AudioFormat error category.
const AudioFormatErrorCategory audioFormatErrorCategory_;

/// Throws a std::system_error in the AudioFormatErrorCategory if result != kAudio_NoError.
/// @param result An OSStatus result code.
/// @param operation An optional string describing the operation that produced result.
/// @throw std::system_error in the AudioFormatErrorCategory.
inline void ThrowIfAudioFormatError(OSStatus result, const char * const operation = nullptr)
{
	if(__builtin_expect(result != kAudio_NoError, false))
		throw std::system_error(result, audioFormatErrorCategory_, operation);
}

} /* namespace */

UInt32 CXXAudioToolbox::ATAudioFormat::GetPropertyInfo(AudioFormatPropertyID inPropertyID, UInt32 inSpecifierSize, const void *inSpecifier)
{
	UInt32 size;
	auto result = AudioFormatGetPropertyInfo(inPropertyID, inSpecifierSize, inSpecifier, &size);
	ThrowIfAudioFormatError(result, "AudioFormatGetPropertyInfo");
	return size;
}

void CXXAudioToolbox::ATAudioFormat::GetProperty(AudioFormatPropertyID inPropertyID, UInt32 inSpecifierSize, const void *inSpecifier, UInt32& ioPropertyDataSize, void *outPropertyData)
{
	auto result = AudioFormatGetProperty(inPropertyID, inSpecifierSize, inSpecifier, &ioPropertyDataSize, outPropertyData);
	ThrowIfAudioFormatError(result, "AudioFormatGetProperty");
}

std::vector<AudioFormatID> CXXAudioToolbox::ATAudioFormat::EncodeFormatIDs()
{
	auto size = GetPropertyInfo(kAudioFormatProperty_EncodeFormatIDs, 0, nullptr);
	auto count = size / sizeof(AudioFormatID);
	auto formatIDs = std::vector<AudioFormatID>(count);
	GetProperty(kAudioFormatProperty_EncodeFormatIDs, 0, nullptr, size, formatIDs.data());
	return formatIDs;
}

std::vector<AudioFormatID> CXXAudioToolbox::ATAudioFormat::DecodeFormatIDs()
{
	auto size = GetPropertyInfo(kAudioFormatProperty_DecodeFormatIDs, 0, nullptr);
	auto count = size / sizeof(AudioFormatID);
	auto formatIDs = std::vector<AudioFormatID>(count);
	GetProperty(kAudioFormatProperty_DecodeFormatIDs, 0, nullptr, size, formatIDs.data());
	return formatIDs;
}
