//
// Copyright © 2021-2025 Stephen F. Booth
// Part of https://github.com/sbooth/CXXAudioToolbox
// MIT license
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
		if(this != &other)
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

	/// Disposes of the managed ExtAudioFile object and replaces it with extAudioFile.
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
	ExtAudioFileRef _Nullable release() noexcept
	{
		return std::exchange(extAudioFile_, nullptr);
	}

private:
	/// The managed ExtAudioFile object.
	ExtAudioFileRef _Nullable extAudioFile_{nullptr};
};

} /* namespace CXXAudioToolbox */
