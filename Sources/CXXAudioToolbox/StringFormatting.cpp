//
// Copyright (c) 2021-2025 Stephen F. Booth
// Part of https://github.com/sbooth/CXXAudioToolbox
// MIT license
//

#import <cctype>
#import <memory>

#import "StringFormatting.hpp"

namespace {

/// Returns true if @c i consists of four printing characters.
constexpr bool fourcc_isprint(uint32_t i) noexcept
{
	return std::isprint(static_cast<unsigned char>(i)) && std::isprint(static_cast<unsigned char>(i >> 8)) && std::isprint(static_cast<unsigned char>(i >> 16)) && std::isprint(static_cast<unsigned char>(i >> 24));
}

/// Creates a @c std::string containing @c fourcc formatted as four characters and returns the result.
/// @throw @c std::length_error
/// @throw @c std::bad_alloc
/// @throw @c std::bad_array_new_length
std::string fourcc_fourchar_string(uint32_t fourcc)
{
	return {
#if __BIG_ENDIAN__
		static_cast<char>(fourcc),
		static_cast<char>(fourcc >> 8),
		static_cast<char>(fourcc >> 16),
		static_cast<char>(fourcc >> 24),
#elif __LITTLE_ENDIAN__
		static_cast<char>(fourcc >> 24),
		static_cast<char>(fourcc >> 16),
		static_cast<char>(fourcc >> 8),
		static_cast<char>(fourcc),
#else
	#error "Unknown endianness"
#endif
	};
}

} /* namespace */

std::string CXXAudioToolbox::concat(std::initializer_list<std::string_view> il)
{
	std::string::size_type len = 0;
	for(auto s : il)
		len += s.size();
	std::string result;
	result.reserve(len);
	for(auto s : il)
		result.append(s);
	return result;
}

std::string CXXAudioToolbox::string_from_cfstring(CFStringRef str)
{
	if(!str)
		return "(null)";

	auto range = CFRange{ .location = 0, .length = CFStringGetLength(str) };
	const auto max_size = CFStringGetMaximumSizeForEncoding(range.length, kCFStringEncodingUTF8);

	std::string result;
	result.reserve(max_size);

	char buf [512];
	CFIndex bytesWritten{0};
	while(range.length > 0) {
		const auto converted = CFStringGetBytes(str, range, kCFStringEncodingUTF8, 0, false, reinterpret_cast<UInt8 *>(buf), sizeof buf, &bytesWritten);
		result.append(buf, static_cast<std::string::size_type>(bytesWritten));
		range.location += converted;
		range.length -= converted;
	}

	return result;
}

std::string CXXAudioToolbox::string_from_cftype(CFTypeRef cf)
{
	if(!cf)
		return "(null)";

	struct cf_type_ref_deleter {
		void operator()(CFTypeRef cf CF_RELEASES_ARGUMENT) { CFRelease(cf); }
	};

	auto description = std::unique_ptr<std::remove_pointer_t<CFStringRef>, cf_type_ref_deleter>{CFCopyDescription(cf)};
	return CXXAudioToolbox::string_from_cfstring(description.get());
}

std::string CXXAudioToolbox::fourcc_string(uint32_t fourcc)
{
	if(fourcc_isprint(fourcc))
		return concat({"'", fourcc_fourchar_string(fourcc), "'"});
	else
		return concat({"0x", to_hex_string(fourcc)});
}

std::string CXXAudioToolbox::osstatus_string(int32_t code)
{
	if(fourcc_isprint(static_cast<uint32_t>(code)))
		return concat({"'", fourcc_fourchar_string(static_cast<uint32_t>(code)), "'"});
	else if(code > -200000 && code < 200000)
		return std::to_string(code);
	else
		return concat({"0x", to_hex_string(static_cast<uint32_t>(code))});
}
