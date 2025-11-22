//
// Copyright © 2021-2025 Stephen F. Booth
// Part of https://github.com/sbooth/CXXAudioToolbox
// MIT license
//

#import "CAAudioFormat.hpp"
#import "AudioToolboxErrors.hpp"

UInt32 CXXAudioToolbox::CAAudioFormat::GetPropertyInfo(AudioFormatPropertyID inPropertyID, UInt32 inSpecifierSize, const void *inSpecifier)
{
	UInt32 size;
	const auto result = AudioFormatGetPropertyInfo(inPropertyID, inSpecifierSize, inSpecifier, &size);
	ThrowIfAudioFormatError(result, "AudioFormatGetPropertyInfo");
	return size;
}

void CXXAudioToolbox::CAAudioFormat::GetProperty(AudioFormatPropertyID inPropertyID, UInt32 inSpecifierSize, const void *inSpecifier, UInt32& ioPropertyDataSize, void *outPropertyData)
{
	const auto result = AudioFormatGetProperty(inPropertyID, inSpecifierSize, inSpecifier, &ioPropertyDataSize, outPropertyData);
	ThrowIfAudioFormatError(result, "AudioFormatGetProperty");
}

std::vector<AudioFormatID> CXXAudioToolbox::CAAudioFormat::EncodeFormatIDs()
{
	auto size = GetPropertyInfo(kAudioFormatProperty_EncodeFormatIDs, 0, nullptr);
	auto count = size / sizeof(AudioFormatID);
	auto formatIDs = std::vector<AudioFormatID>(count);
	GetProperty(kAudioFormatProperty_EncodeFormatIDs, 0, nullptr, size, formatIDs.data());
	return formatIDs;
}

std::vector<AudioFormatID> CXXAudioToolbox::CAAudioFormat::DecodeFormatIDs()
{
	auto size = GetPropertyInfo(kAudioFormatProperty_DecodeFormatIDs, 0, nullptr);
	auto count = size / sizeof(AudioFormatID);
	auto formatIDs = std::vector<AudioFormatID>(count);
	GetProperty(kAudioFormatProperty_DecodeFormatIDs, 0, nullptr, size, formatIDs.data());
	return formatIDs;
}
