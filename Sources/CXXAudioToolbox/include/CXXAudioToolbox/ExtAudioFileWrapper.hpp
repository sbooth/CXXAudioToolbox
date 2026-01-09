//
// SPDX-FileCopyrightText: 2021 Stephen F. Booth <contact@sbooth.dev>
// SPDX-License-Identifier: MIT
//
// Part of https://github.com/sbooth/CXXAudioToolbox
//

#pragma once

#import <utility>

#import <AudioToolbox/ExtendedAudioFile.h>

namespace CXXAudioToolbox {

/// A bare-bones ExtAudioFile wrapper modeled after std::unique_ptr.
class ExtAudioFileWrapper final {
public:
	/// Creates an empty extended audio file wrapper.
	ExtAudioFileWrapper() noexcept = default;

	// This class is non-copyable
	ExtAudioFileWrapper(const ExtAudioFileWrapper&) = delete;

	// This class is non-assignable
	ExtAudioFileWrapper& operator=(const ExtAudioFileWrapper&) = delete;

	/// Move constructor.
	ExtAudioFileWrapper(ExtAudioFileWrapper&& other) noexcept
	: extAudioFile_{other.release()}
	{}

	/// Move assignment operator.
	ExtAudioFileWrapper& operator=(ExtAudioFileWrapper&& other) noexcept
	{
		reset(other.release());
		return *this;
	}

	/// Calls ExtAudioFileDispose on the managed ExtAudioFile object.
	~ExtAudioFileWrapper() noexcept
	{
		reset();
	}

	/// Creates an extended audio file wrapper managing an existing ExtAudioFile object.
	explicit ExtAudioFileWrapper(ExtAudioFileRef _Nullable extAudioFile) noexcept
	: extAudioFile_{extAudioFile}
	{}

	/// Returns true if the managed ExtAudioFile object is not null.
	explicit operator bool() const noexcept
	{
		return extAudioFile_ != nullptr;
	}

	/// Returns the managed ExtAudioFile object.
	operator ExtAudioFileRef _Nullable() const noexcept
	{
		return extAudioFile_;
	}

	/// Returns the managed ExtAudioFile object.
	ExtAudioFileRef _Nullable get() const noexcept
	{
		return extAudioFile_;
	}

	/// Replaces the managed ExtAudioFile object with another ExtAudioFile object.
	/// @note The object assumes responsibility for disposing of the passed ExtAudioFile object using ExtAudioFileDispose.
	void reset(ExtAudioFileRef _Nullable extAudioFile = nullptr) noexcept
	{
		if(auto oldExtAudioFile = std::exchange(extAudioFile_, extAudioFile); oldExtAudioFile)
			ExtAudioFileDispose(oldExtAudioFile);
	}

	/// Swaps the managed ExtAudioFile object with the managed ExtAudioFile object from another extended audio file wrapper.
	void swap(ExtAudioFileWrapper& other) noexcept
	{
		std::swap(extAudioFile_, other.extAudioFile_);
	}

	/// Releases ownership of the managed ExtAudioFile object and returns it.
	/// @note The caller assumes responsibility for disposing of the returned ExtAudioFile object using ExtAudioFileDispose.
	ExtAudioFileRef _Nullable release() noexcept
	{
		return std::exchange(extAudioFile_, nullptr);
	}

private:
	/// The managed ExtAudioFile object.
	ExtAudioFileRef _Nullable extAudioFile_{nullptr};
};

} /* namespace CXXAudioToolbox */
