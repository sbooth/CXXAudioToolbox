//
// SPDX-FileCopyrightText: 2024 Stephen F. Booth <contact@sbooth.dev>
// SPDX-License-Identifier: MIT
//
// Part of https://github.com/sbooth/CXXAudioToolbox
//

#pragma once

#include <AudioToolbox/AUGraph.h>
#include <AudioToolbox/AudioCodec.h>
#include <AudioToolbox/AudioConverter.h>
#include <AudioToolbox/AudioFile.h>
#include <AudioToolbox/AudioFormat.h>
#include <AudioToolbox/ExtendedAudioFile.h>

#include <exception>
#include <system_error>

namespace CXXAudioToolbox {
namespace detail {

/// A std::error_category for return values from the AudioCodec API.
class AudioCodecErrorCategory : public std::error_category {
  public:
    virtual const char *name() const noexcept override final {
        return "AudioCodec";
    }
    virtual std::string message(int condition) const override final {
        switch (static_cast<OSStatus>(condition)) {
            // CoreAudioBaseTypes.h
        case kAudio_NoError:
            return "The function call completed successfully";
        case kAudio_UnimplementedError:
            return "Unimplemented core routine";
        case kAudio_FileNotFoundError:
            return "File not found";
        case kAudio_FilePermissionError:
            return "File cannot be opened due to either file, directory, or sandbox permissions";
        case kAudio_TooManyFilesOpenError:
            return "File cannot be opened because too many files are already open";
        case kAudio_BadFilePathError:
            return "File cannot be opened because the specified path is malformed";
        case kAudio_ParamError:
            return "Error in user parameter list";
        case kAudio_MemFullError:
            return "Not enough room in heap zone";
            // AudioCodec.h
        case kAudioCodecUnspecifiedError:
            return "kAudioCodecUnspecifiedError";
        case kAudioCodecUnknownPropertyError:
            return "kAudioCodecUnknownPropertyError";
        case kAudioCodecBadPropertySizeError:
            return "kAudioCodecBadPropertySizeError";
        case kAudioCodecIllegalOperationError:
            return "kAudioCodecIllegalOperationError";
        case kAudioCodecUnsupportedFormatError:
            return "kAudioCodecUnsupportedFormatError";
        case kAudioCodecStateError:
            return "kAudioCodecStateError";
        case kAudioCodecNotEnoughBufferSpaceError:
            return "kAudioCodecNotEnoughBufferSpaceError";
        case kAudioCodecBadDataError:
            return "kAudioCodecBadDataError";
        default:
            return "Unknown AudioCodec error";
        }
    }
};

/// Global instance of AudioCodec error category.
const AudioCodecErrorCategory audioCodecErrorCategory_;

/// A std::error_category for return values from the AudioConverter API.
class AudioConverterErrorCategory : public std::error_category {
  public:
    virtual const char *name() const noexcept override final {
        return "AudioConverter";
    }
    virtual std::string message(int condition) const override final {
        switch (static_cast<OSStatus>(condition)) {
            // CoreAudioBaseTypes.h
        case kAudio_NoError:
            return "The function call completed successfully";
        case kAudio_UnimplementedError:
            return "Unimplemented core routine";
        case kAudio_FileNotFoundError:
            return "File not found";
        case kAudio_FilePermissionError:
            return "File cannot be opened due to either file, directory, or sandbox permissions";
        case kAudio_TooManyFilesOpenError:
            return "File cannot be opened because too many files are already open";
        case kAudio_BadFilePathError:
            return "File cannot be opened because the specified path is malformed";
        case kAudio_ParamError:
            return "Error in user parameter list";
        case kAudio_MemFullError:
            return "Not enough room in heap zone";
            // AudioConverter.h
        case kAudioConverterErr_FormatNotSupported:
            return "kAudioConverterErr_FormatNotSupported or kAudioFileUnsupportedDataFormatError";
        case kAudioConverterErr_OperationNotSupported:
            return "kAudioConverterErr_OperationNotSupported";
        case kAudioConverterErr_PropertyNotSupported:
            return "kAudioConverterErr_PropertyNotSupported";
        case kAudioConverterErr_InvalidInputSize:
            return "kAudioConverterErr_InvalidInputSize";
        case kAudioConverterErr_InvalidOutputSize:
            return "kAudioConverterErr_InvalidOutputSize";
        case kAudioConverterErr_UnspecifiedError:
            return "kAudioConverterErr_UnspecifiedError";
        case kAudioConverterErr_BadPropertySizeError:
            return "kAudioConverterErr_BadPropertySizeError";
        case kAudioConverterErr_RequiresPacketDescriptionsError:
            return "kAudioConverterErr_RequiresPacketDescriptionsError";
        case kAudioConverterErr_InputSampleRateOutOfRange:
            return "kAudioConverterErr_InputSampleRateOutOfRange";
        case kAudioConverterErr_OutputSampleRateOutOfRange:
            return "kAudioConverterErr_OutputSampleRateOutOfRange";
#if TARGET_OS_IPHONE
        case kAudioConverterErr_HardwareInUse:
            return "kAudioConverterErr_HardwareInUse";
        case kAudioConverterErr_NoHardwarePermission:
            return "kAudioConverterErr_NoHardwarePermission";
#endif /* TARGET_OS_IPHONE */
        default:
            return "Unknown AudioConverter error";
        }
    }
};

/// Global instance of AudioConverter error category.
const AudioConverterErrorCategory audioConverterErrorCategory_;

/// A std::error_category for return values from the AudioFile API.
class AudioFileErrorCategory : public std::error_category {
  public:
    virtual const char *name() const noexcept override final {
        return "AudioFile";
    }
    virtual std::string message(int condition) const override final {
        switch (static_cast<OSStatus>(condition)) {
            // CoreAudioBaseTypes.h
        case kAudio_NoError:
            return "The function call completed successfully";
        case kAudio_UnimplementedError:
            return "Unimplemented core routine";
        case kAudio_FileNotFoundError:
            return "File not found";
        case kAudio_FilePermissionError:
            return "File cannot be opened due to either file, directory, or sandbox permissions";
        case kAudio_TooManyFilesOpenError:
            return "File cannot be opened because too many files are already open";
        case kAudio_BadFilePathError:
            return "File cannot be opened because the specified path is malformed";
        case kAudio_ParamError:
            return "Error in user parameter list";
        case kAudio_MemFullError:
            return "Not enough room in heap zone";
            // AudioFile.h
        case kAudioFileUnspecifiedError:
            return "An unspecified error has occurred";
        case kAudioFileUnsupportedFileTypeError:
            return "The file type is not supported";
        case kAudioFileUnsupportedDataFormatError:
            return "The data format is not supported by this file type";
        case kAudioFileUnsupportedPropertyError:
            return "The property is not supported";
        case kAudioFileBadPropertySizeError:
            return "The size of the property data was not correct";
        case kAudioFilePermissionsError:
            return "The operation violated the file permissions";
        case kAudioFileNotOptimizedError:
            return "There are chunks following the audio data chunk that prevent extending the audio data chunk. The "
                   "file must be optimized in order to write more audio data.";
        case kAudioFileInvalidChunkError:
            return "The chunk does not exist in the file or is not supported by the file";
        case kAudioFileDoesNotAllow64BitDataSizeError:
            return "The a file offset was too large for the file type. AIFF and WAVE have a 32 bit file size limit.";
        case kAudioFileInvalidPacketOffsetError:
            return "A packet offset was past the end of the file, or not at the end of the file when writing a VBR "
                   "format, or a corrupt packet size was read when building the packet table.";
        case kAudioFileInvalidPacketDependencyError:
            return "Either the packet dependency info that's necessary for the audio format has not been provided, or "
                   "the provided packet dependency info indicates dependency on a packet that's unavailable.";
        case kAudioFileInvalidFileError:
            return "The file is malformed, or otherwise not a valid instance of an audio file of its type";
        case kAudioFileOperationNotSupportedError:
            return "The operation cannot be performed";
        case kAudioFileNotOpenError:
            return "The file is closed";
        case kAudioFileEndOfFileError:
            return "End of file";
        case kAudioFilePositionError:
            return "Invalid file position";
            //			case kAudioFileFileNotFoundError: 				return "File not found";
        default:
            return "Unknown AudioFile error";
        }
    }
};

/// Global instance of AudioFile error category.
const AudioFileErrorCategory audioFileErrorCategory_;

/// A std::error_category for return values from the AudioFormat API.
class AudioFormatErrorCategory : public std::error_category {
  public:
    virtual const char *name() const noexcept override final {
        return "AudioFormat";
    }
    virtual std::string message(int condition) const override final {
        switch (static_cast<OSStatus>(condition)) {
            // CoreAudioBaseTypes.h
        case kAudio_NoError:
            return "The function call completed successfully";
        case kAudio_UnimplementedError:
            return "Unimplemented core routine";
        case kAudio_FileNotFoundError:
            return "File not found";
        case kAudio_FilePermissionError:
            return "File cannot be opened due to either file, directory, or sandbox permissions";
        case kAudio_TooManyFilesOpenError:
            return "File cannot be opened because too many files are already open";
        case kAudio_BadFilePathError:
            return "File cannot be opened because the specified path is malformed";
        case kAudio_ParamError:
            return "Error in user parameter list";
        case kAudio_MemFullError:
            return "Not enough room in heap zone";
            // AudioFormat.h
        case kAudioFormatUnspecifiedError:
            return "kAudioFormatUnspecifiedError";
        case kAudioFormatUnsupportedPropertyError:
            return "kAudioFormatUnsupportedPropertyError";
        case kAudioFormatBadPropertySizeError:
            return "kAudioFormatBadPropertySizeError";
        case kAudioFormatBadSpecifierSizeError:
            return "kAudioFormatBadSpecifierSizeError";
        case kAudioFormatUnsupportedDataFormatError:
            return "kAudioFormatUnsupportedDataFormatError";
        case kAudioFormatUnknownFormatError:
            return "kAudioFormatUnknownFormatError";
        default:
            return "Unknown AudioFormat error";
        }
    }
};

/// Global instance of AudioFormat error category.
const AudioFormatErrorCategory audioFormatErrorCategory_;

/// A std::error_category for return values from the AudioUnit API.
class AudioUnitErrorCategory : public std::error_category {
  public:
    virtual const char *name() const noexcept override final {
        return "AudioUnit";
    }
    virtual std::string message(int condition) const override final {
        switch (static_cast<OSStatus>(condition)) {
            // CoreAudioBaseTypes.h
        case kAudio_NoError:
            return "The function call completed successfully";
        case kAudio_UnimplementedError:
            return "Unimplemented core routine";
        case kAudio_FileNotFoundError:
            return "File not found";
        case kAudio_FilePermissionError:
            return "File cannot be opened due to either file, directory, or sandbox permissions";
        case kAudio_TooManyFilesOpenError:
            return "File cannot be opened because too many files are already open";
        case kAudio_BadFilePathError:
            return "File cannot be opened because the specified path is malformed";
        case kAudio_ParamError:
            return "Error in user parameter list";
        case kAudio_MemFullError:
            return "Not enough room in heap zone";
            // AUComponent.h
        case kAudioUnitErr_InvalidProperty:
            return "The property is not supported";
        case kAudioUnitErr_InvalidParameter:
            return "The parameter is not supported";
        case kAudioUnitErr_InvalidElement:
            return "The specified element is not valid";
        case kAudioUnitErr_NoConnection:
            return "There is no connection (generally an audio unit is asked to render but it has not input from which "
                   "to gather data)";
        case kAudioUnitErr_FailedInitialization:
            return "The audio unit is unable to be initialized";
        case kAudioUnitErr_TooManyFramesToProcess:
            return "When an audio unit is initialized it has a value which specifies the max number of frames it will "
                   "be asked to render at any given time. If an audio unit is asked to render more than this, this "
                   "error is returned";
        case kAudioUnitErr_InvalidFile:
            return "If an audio unit uses external files as a data source, this error is returned if a file is invalid "
                   "(Apple's DLS synth returns this error)";
        case kAudioUnitErr_UnknownFileType:
            return "If an audio unit uses external files as a data source, this error is returned if a file is invalid "
                   "(Apple's DLS synth returns this error)";
        case kAudioUnitErr_FileNotSpecified:
            return "If an audio unit uses external files as a data source, this error is returned if a file hasn't "
                   "been set on it (Apple's DLS synth returns this error)";
        case kAudioUnitErr_FormatNotSupported:
            return "Returned if an input or output format is not supported";
        case kAudioUnitErr_Uninitialized:
            return "Returned if an operation requires an audio unit to be initialized and it is not";
        case kAudioUnitErr_InvalidScope:
            return "The specified scope is invalid";
        case kAudioUnitErr_PropertyNotWritable:
            return "The property cannot be written";
        case kAudioUnitErr_CannotDoInCurrentContext:
            return "Returned when an audio unit is in a state where it can't perform the requested action now - but it "
                   "could later. It's usually used to guard a render operation when a reconfiguration of its internal "
                   "state is being performed";
        case kAudioUnitErr_InvalidPropertyValue:
            return "The property is valid, but the value of the property being provided is not";
        case kAudioUnitErr_PropertyNotInUse:
            return "Returned when a property is valid, but it hasn't been set to a valid value at this time";
        case kAudioUnitErr_Initialized:
            return "Indicates the operation cannot be performed because the audio unit is initialized";
        case kAudioUnitErr_InvalidOfflineRender:
            return "Used to indicate that the offline render operation is invalid. For instance, when the audio unit "
                   "needs to be pre-flighted, but it hasn't been";
        case kAudioUnitErr_Unauthorized:
            return "Returned by either Open or Initialize, this error is used to indicate that the audio unit is not "
                   "authorised, that it cannot be used. A host can then present a UI to notify the user the audio unit "
                   "is not able to be used in its current state";
        case kAudioUnitErr_MIDIOutputBufferFull:
            return "Returned during the render call, if the audio unit produces more MIDI output, than the default "
                   "allocated buffer. The audio unit can provide a size hint, in case it needs a larger buffer. See "
                   "the documentation for AUAudioUnit's MIDIOutputBufferSizeHint property";
        case kAudioComponentErr_InstanceTimedOut:
            return "kAudioComponentErr_InstanceTimedOut";
        case kAudioComponentErr_InstanceInvalidated:
            return "The component instance's implementation is not available, most likely because the process that "
                   "published it is no longer running";
        case kAudioUnitErr_RenderTimeout:
            return "The audio unit did not satisfy the render request in time";
        case kAudioUnitErr_ExtensionNotFound:
            return "The specified identifier did not match any Audio Unit Extensions";
        case kAudioUnitErr_InvalidParameterValue:
            return "The parameter value is not supported, e.g. the value specified is NaN or infinite";
        case kAudioUnitErr_InvalidFilePath:
            return "The file path that was passed is not supported. It is either too long or contains invalid "
                   "characters";
        case kAudioUnitErr_MissingKey:
            return "A required key is missing from a dictionary object";
        case kAudioComponentErr_DuplicateDescription:
            return "A non-unique component description was provided to AudioOutputUnitPublish";
        case kAudioComponentErr_UnsupportedType:
            return "An unsupported component type was provided to AudioOutputUnitPublish";
        case kAudioComponentErr_TooManyInstances:
            return "Components published via AudioOutputUnitPublish may only have one instance";
        case kAudioComponentErr_NotPermitted:
            return "App needs \"inter-app-audio\" entitlement or host app needs \"audio\" in its UIBackgroundModes. Or "
                   "app is trying to register a component not declared in its Info.plist";
        case kAudioComponentErr_InitializationTimedOut:
            return "Host did not render in a timely manner; must uninitialize and reinitialize";
        case kAudioComponentErr_InvalidFormat:
            return "Inter-app AU element formats must have sample rates matching the hardware";
        default:
            return "Unknown AudioUnit error";
        }
    }
};

/// Global instance of AudioUnit error category.
const AudioUnitErrorCategory audioUnitErrorCategory_;

/// A std::error_category for return values from the AUGraph API.
class AUGraphErrorCategory : public std::error_category {
  public:
    virtual const char *name() const noexcept override final {
        return "AUGraph";
    }
    virtual std::string message(int condition) const override final {
        switch (static_cast<OSStatus>(condition)) {
            // CoreAudioBaseTypes.h
        case kAudio_NoError:
            return "The function call completed successfully";
        case kAudio_UnimplementedError:
            return "Unimplemented core routine";
        case kAudio_FileNotFoundError:
            return "File not found";
        case kAudio_FilePermissionError:
            return "File cannot be opened due to either file, directory, or sandbox permissions";
        case kAudio_TooManyFilesOpenError:
            return "File cannot be opened because too many files are already open";
        case kAudio_BadFilePathError:
            return "File cannot be opened because the specified path is malformed";
        case kAudio_ParamError:
            return "Error in user parameter list";
        case kAudio_MemFullError:
            return "Not enough room in heap zone";
            // AUGraph.h
        case kAUGraphErr_NodeNotFound:
            return "The specified node cannot be found";
        case kAUGraphErr_InvalidConnection:
            return "The attempted connection between two nodes cannot be made";
        case kAUGraphErr_OutputNodeErr:
            return "AUGraphs can only contain one OutputUnit";
        case kAUGraphErr_CannotDoInCurrentContext:
            return "The required lock is held by another thread";
        case kAUGraphErr_InvalidAudioUnit:
            return "The audio unit is invalid";
        default:
            return "Unknown AUGraph error";
        }
    }
};

/// Global instance of AUGraph error category.
const AUGraphErrorCategory auGraphErrorCategory_;

/// A std::error_category for return values from the ExtAudioFile API.
class ExtAudioFileErrorCategory : public std::error_category {
  public:
    virtual const char *name() const noexcept override final {
        return "ExtAudioFile";
    }
    virtual std::string message(int condition) const override final {
        switch (static_cast<OSStatus>(condition)) {
            // CoreAudioBaseTypes.h
        case kAudio_NoError:
            return "The function call completed successfully";
        case kAudio_UnimplementedError:
            return "Unimplemented core routine";
        case kAudio_FileNotFoundError:
            return "File not found";
        case kAudio_FilePermissionError:
            return "File cannot be opened due to either file, directory, or sandbox permissions";
        case kAudio_TooManyFilesOpenError:
            return "File cannot be opened because too many files are already open";
        case kAudio_BadFilePathError:
            return "File cannot be opened because the specified path is malformed";
        case kAudio_ParamError:
            return "Error in user parameter list";
        case kAudio_MemFullError:
            return "Not enough room in heap zone";
            // ExtAudioFile.h
        case kExtAudioFileError_InvalidProperty:
            return "kExtAudioFileError_InvalidProperty";
        case kExtAudioFileError_InvalidPropertySize:
            return "kExtAudioFileError_InvalidPropertySize";
        case kExtAudioFileError_NonPCMClientFormat:
            return "kExtAudioFileError_NonPCMClientFormat";
        case kExtAudioFileError_InvalidChannelMap:
            return "number of channels doesn't match format";
        case kExtAudioFileError_InvalidOperationOrder:
            return "kExtAudioFileError_InvalidOperationOrder";
        case kExtAudioFileError_InvalidDataFormat:
            return "kExtAudioFileError_InvalidDataFormat";
        case kExtAudioFileError_MaxPacketSizeUnknown:
            return "kExtAudioFileError_MaxPacketSizeUnknown";
        case kExtAudioFileError_InvalidSeek:
            return "writing, or offset out of bounds";
        case kExtAudioFileError_AsyncWriteTooLarge:
            return "kExtAudioFileError_AsyncWriteTooLarge";
        case kExtAudioFileError_AsyncWriteBufferOverflow:
            return "an async write could not be completed in time";
#if TARGET_OS_IPHONE
        case kExtAudioFileError_CodecUnavailableInputConsumed:
            return "iOS only. Returned when ExtAudioFileWrite was interrupted. You must stop calling "
                   "ExtAudioFileWrite. If the underlying audio converter can resume after an interruption (see "
                   "kAudioConverterPropertyCanResumeFromInterruption), you must wait for an EndInterruption "
                   "notification from AudioSession, and call AudioSessionSetActive(true) before resuming. In this "
                   "situation, the buffer you provided to ExtAudioFileWrite was successfully consumed and you may "
                   "proceed to the next buffer";
        case kExtAudioFileError_CodecUnavailableInputNotConsumed:
            return "iOS only. Returned when ExtAudioFileWrite was interrupted. You must stop calling "
                   "ExtAudioFileWrite. If the underlying audio converter can resume after an interruption (see "
                   "kAudioConverterPropertyCanResumeFromInterruption), you must wait for an EndInterruption "
                   "notification from AudioSession, and call AudioSessionSetActive(true) before resuming. In this "
                   "situation, the buffer you provided to ExtAudioFileWrite was not successfully consumed and you must "
                   "try to write it again";
#endif /* TARGET_OS_IPHONE */
        default:
            return "Unknown ExtAudioFile error";
        }
    }
};

/// Global instance of ExtAudioFile error category.
const ExtAudioFileErrorCategory extAudioFileErrorCategory_;

} /* namespace detail */

// MARK: -

/// Throws a std::system_error in the AudioCodecErrorCategory if result != kAudio_NoError.
/// @param result An OSStatus result code.
/// @param operation An optional string describing the operation that produced result.
/// @throw std::system_error in the AudioCodecErrorCategory.
inline void ThrowIfAudioCodecError(OSStatus result, const char *const operation = nullptr) {
    if (__builtin_expect(result != kAudio_NoError, false)) {
        throw std::system_error(result, detail::audioCodecErrorCategory_, operation);
    }
}

/// Throws a std::system_error in the AudioConverterErrorCategory if result != kAudio_NoError.
/// @param result An OSStatus result code.
/// @param operation An optional string describing the operation that produced result.
/// @throw std::system_error in the AudioConverterErrorCategory.
inline void ThrowIfAudioConverterError(OSStatus result, const char *const operation = nullptr) {
    if (__builtin_expect(result != kAudio_NoError, false)) {
        throw std::system_error(result, detail::audioConverterErrorCategory_, operation);
    }
}

/// Throws a std::system_error in the AudioFileErrorCategory if result != kAudio_NoError.
/// @param result An OSStatus result code.
/// @param operation An optional string describing the operation that produced result.
/// @throw std::system_error in the AudioFileErrorCategory.
inline void ThrowIfAudioFileError(OSStatus result, const char *const operation = nullptr) {
    if (__builtin_expect(result != kAudio_NoError, false)) {
        throw std::system_error(result, detail::audioFileErrorCategory_, operation);
    }
}

/// Throws a std::system_error in the AudioFormatErrorCategory if result != kAudio_NoError.
/// @param result An OSStatus result code.
/// @param operation An optional string describing the operation that produced result.
/// @throw std::system_error in the AudioFormatErrorCategory.
inline void ThrowIfAudioFormatError(OSStatus result, const char *const operation = nullptr) {
    if (__builtin_expect(result != kAudio_NoError, false)) {
        throw std::system_error(result, detail::audioFormatErrorCategory_, operation);
    }
}

/// Throws a std::system_error in the AudioUnitErrorCategory if result != kAudio_NoError.
/// @param result An OSStatus result code.
/// @param operation An optional string describing the operation that produced result.
/// @throw std::system_error in the AudioUnitErrorCategory.
inline void ThrowIfAudioUnitError(OSStatus result, const char *const operation = nullptr) {
    if (__builtin_expect(result != kAudio_NoError, false)) {
        throw std::system_error(result, detail::audioUnitErrorCategory_, operation);
    }
}

/// Throws a std::system_error in the AUGraphErrorCategory if result != kAudio_NoError.
/// @param result An OSStatus result code.
/// @param operation An optional string describing the operation that produced result.
/// @throw std::system_error in the AUGraphErrorCategory.
inline void ThrowIfAUGraphError(OSStatus result, const char *const operation = nullptr) {
    if (__builtin_expect(result != kAudio_NoError, false)) {
        throw std::system_error(result, detail::auGraphErrorCategory_, operation);
    }
}

/// Throws a std::system_error in the ExtAudioFileErrorCategory if result != kAudio_NoError.
/// @param result An OSStatus result code.
/// @param operation An optional string describing the operation that produced result.
/// @throw std::system_error in the ExtAudioFileErrorCategory.
inline void ThrowIfExtAudioFileError(OSStatus result, const char *const operation = nullptr) {
    if (__builtin_expect(result != kAudio_NoError, false)) {
        throw std::system_error(result, detail::extAudioFileErrorCategory_, operation);
    }
}

} /* namespace CXXAudioToolbox */
