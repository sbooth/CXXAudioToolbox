//
// Copyright © 2021-2025 Stephen F. Booth
// Part of https://github.com/sbooth/CXXAudioToolbox
// MIT license
//

#pragma once

#import <vector>

#import <AudioToolbox/AudioFormat.h>

CF_ASSUME_NONNULL_BEGIN

namespace CXXAudioToolbox {
namespace ATAudioFormat {

/// Retrieves information about the given property.
/// @param inPropertyID An AudioFormatPropertyID constant.
/// @param inSpecifierSize The size of the specifier data.
/// @param inSpecifier A specifier is a buffer of data used as an input argument to some of the properties.
/// @return The size in bytes of the current value of the property.
/// @throw std::system_error.
UInt32 GetPropertyInfo(AudioFormatPropertyID inPropertyID, UInt32 inSpecifierSize, const void * _Nullable inSpecifier);

/// Retrieves the indicated property data
/// @param inPropertyID An AudioFormatPropertyID constant.
/// @param inSpecifierSize The size of the specifier data.
/// @param inSpecifier A specifier is a buffer of data used as an input argument to some of the properties.
/// @param ioPropertyDataSize On input the size of the outPropertyData buffer. On output the number of bytes written to the buffer.
/// @param outPropertyData The buffer in which to write the property data.
/// @throw std::system_error.
void GetProperty(AudioFormatPropertyID inPropertyID, UInt32 inSpecifierSize, const void * _Nullable inSpecifier, UInt32& ioPropertyDataSize, void * _Nullable outPropertyData);

/// Returns an array of format IDs that are valid output formats for a converter.
/// @throw std::system_error.
std::vector<AudioFormatID> EncodeFormatIDs();

/// Returns an array of format IDs that are valid input formats for a converter.
/// @throw std::system_error.
std::vector<AudioFormatID> DecodeFormatIDs();

} /* namespace ATAudioFormat */
} /* namespace CXXAudioToolbox */

CF_ASSUME_NONNULL_END
