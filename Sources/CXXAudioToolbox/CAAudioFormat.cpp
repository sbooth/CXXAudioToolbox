//
// SPDX-FileCopyrightText: 2021 Stephen F. Booth <contact@sbooth.dev>
// SPDX-License-Identifier: MIT
//
// Part of https://github.com/sbooth/CXXAudioToolbox
//

#import "CAAudioFormat.hpp"
#import "AudioToolboxErrors.hpp"
#import "StringFormatting.hpp"

UInt32 CXXAudioToolbox::CAAudioFormat::GetPropertyInfo(AudioFormatPropertyID inPropertyID, UInt32 inSpecifierSize, const void *inSpecifier)
{
	UInt32 size;
	const auto result = AudioFormatGetPropertyInfo(inPropertyID, inSpecifierSize, inSpecifier, &size);
	ThrowIfAudioFormatError(result, "");
	CXXAudioToolbox_ThrowIfAudioFormatError(result, concat({
		"AudioFormatGetPropertyInfo(",
		to_fourcc_string(inPropertyID),
		", ", to_string(inSpecifierSize),
		", 0x", to_hex_string(reinterpret_cast<uintptr_t>(inSpecifier)),
		")",
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
	return size;
}

void CXXAudioToolbox::CAAudioFormat::GetProperty(AudioFormatPropertyID inPropertyID, UInt32 inSpecifierSize, const void *inSpecifier, UInt32& ioPropertyDataSize, void *outPropertyData)
{
	const auto result = AudioFormatGetProperty(inPropertyID, inSpecifierSize, inSpecifier, &ioPropertyDataSize, outPropertyData);
	ThrowIfAudioFormatError(result, "");
	CXXAudioToolbox_ThrowIfAudioFormatError(result, concat({
		"AudioFormatGetProperty(",
		to_fourcc_string(inPropertyID),
		", ", to_string(inSpecifierSize),
		", 0x", to_hex_string(reinterpret_cast<uintptr_t>(inSpecifier)),
		", ", to_string(ioPropertyDataSize),
		", 0x", to_hex_string(reinterpret_cast<uintptr_t>(outPropertyData)),
		")",
		" [", __FILE_NAME__, ":", to_string(__LINE__), "]"
	}));
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
