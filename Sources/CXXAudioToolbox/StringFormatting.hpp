//
// Copyright (c) 2021-2025 Stephen F. Booth
// Part of https://github.com/sbooth/CXXAudioToolbox
// MIT license
//

#pragma once

#import <algorithm>
#import <initializer_list>
#import <string>
#import <string_view>
#import <type_traits>

#import <CoreFoundation/CoreFoundation.h>

namespace CXXAudioToolbox {

/// Creates a @c std::string by concatenating the contents of an initializer list and returns the result.
/// @param il An initializer list containing the string views to concatenate.
/// @return A @c std::string containing the concatenated result.
/// @throw @c std::length_error
/// @throw @c std::bad_alloc
/// @throw @c std::bad_array_new_length
std::string concat(std::initializer_list<std::string_view> il);

/// Creates a @c std::string containing @c val formatted as a string in the specified radix and returns the result.
/// @tparam R The desired radix in the interval [2, 36].
/// @tparam T An integral type.
/// @param value A value to format.
/// @throw @c std::length_error
/// @throw @c std::bad_alloc
/// @throw @c std::bad_array_new_length
template <std::size_t R = 10, typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
std::string to_string(T value) {
	static_assert(R >= 2 && R <= 36, "Radix out of range");

	constexpr char digits [] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	constexpr auto bits = sizeof(T) * CHAR_BIT;
	// Rather than computing the actual maximum number of digits using logarithms,
	// just use an upper bound based on the number of bits in the number
	// and add one for the sign.
	// The actual maximum number of digits for an N-bit number in base B is
	// ceil(N*logB(2))
	constexpr auto max_len = bits + 1;

	char buffer[max_len];
	char *end = buffer + sizeof buffer;
	char *p = end;

	using U = std::make_unsigned_t<T>;

	/// Writes digits of @c v to @c p
	auto write_digits = [&p](U v) {
		do {
			*--p = digits[v % R];
			v /= R;
		} while(v != 0);
	};

	if constexpr (std::is_signed_v<T>) {
		if(value < 0) {
			write_digits(static_cast<U>(-(value + 1)) + 1);
			*--p = '-';
		}
		else
			write_digits(static_cast<U>(value));
	}
	else
		write_digits(value);

	return std::string(p, end - p);
}

/// Creates a @c std::string containing @c val formatted as hexadecimal and returns the result.
/// @tparam T An unsigned type
/// @param val A value to format.
/// @param len The desired length of the output string.
/// @throw @c std::length_error
/// @throw @c std::bad_alloc
/// @throw @c std::bad_array_new_length
template <typename T, typename = std::enable_if_t<std::is_unsigned_v<T>>>
std::string to_hex_string(T val, std::string::size_type len = sizeof(T) << 1)
{
	constexpr char digits [] = "0123456789ABCDEF";
	len = std::min(len, sizeof(T) << 1);
	std::string result(len, '0');
	for(std::string::size_type i = 0, j = (len - 1) * 4; i < len; ++i, j -= 4)
		result[i] = digits[(val >> j) & 0x0f];
	return result;
}

/// Creates a @c std::string from a Core Foundation string using UTF-8 and returns the result.
/// @param str A Core Foundation string containing the characters to copy.
/// @return A @c std::string containing the contents of @c str in UTF-8.
/// @throw @c std::length_error
/// @throw @c std::bad_alloc
/// @throw @c std::bad_array_new_length
std::string string_from_cfstring(CFStringRef _Nullable str);

/// Creates a @c std::string from the description of a Core Foundation type using UTF-8 and returns the result.
/// @param cf A Core Foundation object.
/// @return A @c std::string containing the description of @c cf in UTF-8.
/// @throw @c std::length_error
/// @throw @c std::bad_alloc
/// @throw @c std::bad_array_new_length
std::string string_from_cftype(CFTypeRef _Nullable cf);

/// Creates a string representation of a four-character code and returns the result.
/// @param fourcc A four-character code.
/// @return A @c std::string containing a string representation of @c fourcc.
/// @throw @c std::length_error
/// @throw @c std::bad_alloc
/// @throw @c std::bad_array_new_length
std::string fourcc_string(uint32_t fourcc);

/// Creates a string representation of an @c OSStatus result code and returns the result.
/// @param code An @c OSStatus result code.
/// @return A @c std::string containing a string representation of code.
/// @throw @c std::length_error
/// @throw @c std::bad_alloc
/// @throw @c std::bad_array_new_length
std::string osstatus_string(int32_t code);

} /* namespace CXXAudioToolbox */
