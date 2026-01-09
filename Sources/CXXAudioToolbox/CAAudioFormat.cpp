//
// SPDX-FileCopyrightText: 2021 Stephen F. Booth <contact@sbooth.dev>
// SPDX-License-Identifier: MIT
//
// Part of https://github.com/sbooth/CXXAudioToolbox
//

#import "CAAudioFormat.hpp"

std::expected<UInt32, OSStatus> CXXAudioToolbox::CAAudioFormat::GetPropertyInfo(AudioFormatPropertyID inPropertyID, UInt32 inSpecifierSize, const void *inSpecifier) noexcept
{
	UInt32 size;
	const auto result = AudioFormatGetPropertyInfo(inPropertyID, inSpecifierSize, inSpecifier, &size);
	if(result != noErr)
		return std::unexpected(result);
	return size;
}

std::expected<void, OSStatus> CXXAudioToolbox::CAAudioFormat::GetProperty(AudioFormatPropertyID inPropertyID, UInt32 inSpecifierSize, const void *inSpecifier, UInt32& ioPropertyDataSize, void *outPropertyData) noexcept
{
	const auto result = AudioFormatGetProperty(inPropertyID, inSpecifierSize, inSpecifier, &ioPropertyDataSize, outPropertyData);
	if(result != noErr)
		return std::unexpected(result);
	return {};
}

std::expected<std::vector<AudioFormatID>, OSStatus> CXXAudioToolbox::CAAudioFormat::EncodeFormatIDs() noexcept
{
	const auto get_property_encode_format_ids = [&](UInt32 size) -> std::expected<std::vector<AudioFormatID>, OSStatus> {
		try {
			const auto count = size / sizeof(AudioFormatID);
			auto formatIDs = std::vector<AudioFormatID>(count);
			return GetProperty(kAudioFormatProperty_EncodeFormatIDs, 0, nullptr, size, formatIDs.data()).and_then([&] { return formatIDs; });
		} catch(const std::exception& e) {
			return std::unexpected(kAudio_MemFullError);
		}
	};

	return GetPropertyInfo(kAudioFormatProperty_EncodeFormatIDs, 0, nullptr).and_then(get_property_encode_format_ids);
}

std::expected<std::vector<AudioFormatID>, OSStatus> CXXAudioToolbox::CAAudioFormat::DecodeFormatIDs() noexcept
{
	const auto get_property_encode_format_ids = [&](UInt32 size) -> std::expected<std::vector<AudioFormatID>, OSStatus> {
		try {
			const auto count = size / sizeof(AudioFormatID);
			auto formatIDs = std::vector<AudioFormatID>(count);
			return GetProperty(kAudioFormatProperty_DecodeFormatIDs, 0, nullptr, size, formatIDs.data()).and_then([&] { return formatIDs; });
		} catch(const std::exception& e) {
			return std::unexpected(kAudio_MemFullError);
		}
	};

	return GetPropertyInfo(kAudioFormatProperty_DecodeFormatIDs, 0, nullptr).and_then(get_property_encode_format_ids);
}
