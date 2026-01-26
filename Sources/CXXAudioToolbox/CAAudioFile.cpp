//
// SPDX-FileCopyrightText: 2021 Stephen F. Booth <contact@sbooth.dev>
// SPDX-License-Identifier: MIT
//
// Part of https://github.com/sbooth/CXXAudioToolbox
//

#include "CXXAudioToolbox/CAAudioFile.hpp"

#include "AudioToolboxErrors.hpp"

CXXAudioToolbox::CAAudioFile::CAAudioFile(CAAudioFile&& other) noexcept
  : audioFile_{other.release()} {}

CXXAudioToolbox::CAAudioFile& CXXAudioToolbox::CAAudioFile::operator=(CAAudioFile&& other) noexcept {
    reset(other.release());
    return *this;
}

/// Destroys the audio file and releases all associated resources.
CXXAudioToolbox::CAAudioFile::~CAAudioFile() noexcept {
    reset();
}

void CXXAudioToolbox::CAAudioFile::OpenURL(CFURLRef inURL, AudioFilePermissions inPermissions,
                                           AudioFileTypeID inFileTypeHint) {
    Close();
    const auto result = AudioFileOpenURL(inURL, inPermissions, inFileTypeHint, &audioFile_);
    ThrowIfAudioFileError(result, "AudioFileOpenURL");
}

void CXXAudioToolbox::CAAudioFile::CreateWithURL(CFURLRef inURL, AudioFileTypeID inFileType,
                                                 const AudioStreamBasicDescription& inFormat, AudioFileFlags inFlags) {
    Close();
    const auto result = AudioFileCreateWithURL(inURL, inFileType, &inFormat, inFlags, &audioFile_);
    ThrowIfAudioFileError(result, "AudioFileCreateWithURL");
}

void CXXAudioToolbox::CAAudioFile::InitializeWithCallbacks(
      void *inClientData, AudioFile_ReadProc inReadFunc, AudioFile_WriteProc inWriteFunc,
      AudioFile_GetSizeProc inGetSizeFunc, AudioFile_SetSizeProc inSetSizeFunc, AudioFileTypeID inFileType,
      const AudioStreamBasicDescription& inFormat, AudioFileFlags inFlags) {
    Close();
    const auto result = AudioFileInitializeWithCallbacks(inClientData, inReadFunc, inWriteFunc, inGetSizeFunc,
                                                         inSetSizeFunc, inFileType, &inFormat, inFlags, &audioFile_);
    ThrowIfAudioFileError(result, "AudioFileInitializeWithCallbacks");
}

void CXXAudioToolbox::CAAudioFile::OpenWithCallbacks(void *inClientData, AudioFile_ReadProc inReadFunc,
                                                     AudioFile_WriteProc _Nullable inWriteFunc,
                                                     AudioFile_GetSizeProc inGetSizeFunc,
                                                     AudioFile_SetSizeProc _Nullable inSetSizeFunc,
                                                     AudioFileTypeID inFileTypeHint) {
    Close();
    const auto result = AudioFileOpenWithCallbacks(inClientData, inReadFunc, inWriteFunc, inGetSizeFunc, inSetSizeFunc,
                                                   inFileTypeHint, &audioFile_);
    ThrowIfAudioFileError(result, "AudioFileOpenWithCallbacks");
}

void CXXAudioToolbox::CAAudioFile::Close() {
    if (audioFile_) {
        const auto result = AudioFileClose(audioFile_);
        audioFile_ = nullptr;
        ThrowIfAudioFileError(result, "AudioFileClose");
    }
}

void CXXAudioToolbox::CAAudioFile::Optimize() {
    const auto result = AudioFileOptimize(audioFile_);
    ThrowIfAudioFileError(result, "AudioFileOptimize");
}

OSStatus CXXAudioToolbox::CAAudioFile::ReadBytes(bool inUseCache, SInt64 inStartingByte, UInt32& ioNumBytes,
                                                 void *outBuffer) {
    const auto result = AudioFileReadBytes(audioFile_, inUseCache, inStartingByte, &ioNumBytes, outBuffer);
    switch (result) {
    case noErr:
    case kAudioFileEndOfFileError:
        break;
    default:
        ThrowIfAudioFileError(result, "AudioFileReadBytes");
        break;
    }
    return result;
}

void CXXAudioToolbox::CAAudioFile::WriteBytes(bool inUseCache, SInt64 inStartingByte, UInt32& ioNumBytes,
                                              const void *inBuffer) {
    const auto result = AudioFileWriteBytes(audioFile_, inUseCache, inStartingByte, &ioNumBytes, inBuffer);
    ThrowIfAudioFileError(result, "AudioFileWriteBytes");
}

OSStatus CXXAudioToolbox::CAAudioFile::ReadPacketData(bool inUseCache, UInt32& ioNumBytes,
                                                      AudioStreamPacketDescription *_Nullable outPacketDescriptions,
                                                      SInt64 inStartingPacket, UInt32& ioNumPackets,
                                                      void *_Nullable outBuffer) {
    const auto result = AudioFileReadPacketData(audioFile_, inUseCache, &ioNumBytes, outPacketDescriptions,
                                                inStartingPacket, &ioNumPackets, outBuffer);
    switch (result) {
    case noErr:
    case kAudioFileEndOfFileError:
        break;
    default:
        ThrowIfAudioFileError(result, "AudioFileReadPacketData");
        break;
    }
    return result;
}

void CXXAudioToolbox::CAAudioFile::WritePackets(bool inUseCache, UInt32 inNumBytes,
                                                const AudioStreamPacketDescription *_Nullable inPacketDescriptions,
                                                SInt64 inStartingPacket, UInt32& ioNumPackets, const void *inBuffer) {
    const auto result = AudioFileWritePackets(audioFile_, inUseCache, inNumBytes, inPacketDescriptions,
                                              inStartingPacket, &ioNumPackets, inBuffer);
    ThrowIfAudioFileError(result, "AudioFileWritePackets");
}

UInt32 CXXAudioToolbox::CAAudioFile::GetUserDataSize(UInt32 inUserDataID, UInt32 inIndex) {
    UInt32 size;
    const auto result = AudioFileGetUserDataSize(audioFile_, inUserDataID, inIndex, &size);
    ThrowIfAudioFileError(result, "AudioFileGetUserDataSize");
    return size;
}

void CXXAudioToolbox::CAAudioFile::GetUserData(UInt32 inUserDataID, UInt32 inIndex, UInt32& ioUserDataSize,
                                               void *outUserData) const {
    const auto result = AudioFileGetUserData(audioFile_, inUserDataID, inIndex, &ioUserDataSize, outUserData);
    ThrowIfAudioFileError(result, "AudioFileGetUserData");
}

void CXXAudioToolbox::CAAudioFile::SetUserData(UInt32 inUserDataID, UInt32 inIndex, UInt32 inUserDataSize,
                                               const void *inUserData) {
    const auto result = AudioFileSetUserData(audioFile_, inUserDataID, inIndex, inUserDataSize, inUserData);
    ThrowIfAudioFileError(result, "AudioFileGetUserData");
}

void CXXAudioToolbox::CAAudioFile::RemoveUserData(UInt32 inUserDataID, UInt32 inIndex) {
    const auto result = AudioFileRemoveUserData(audioFile_, inUserDataID, inIndex);
    ThrowIfAudioFileError(result, "AudioFileRemoveUserData");
}

void CXXAudioToolbox::CAAudioFile::GetPropertyInfo(AudioFilePropertyID inPropertyID, UInt32 *_Nullable outDataSize,
                                                   UInt32 *_Nullable isWritable) const {
    const auto result = AudioFileGetPropertyInfo(audioFile_, inPropertyID, outDataSize, isWritable);
    ThrowIfAudioFileError(result, "AudioFileGetPropertyInfo");
}

void CXXAudioToolbox::CAAudioFile::GetProperty(AudioFilePropertyID inPropertyID, UInt32& ioDataSize,
                                               void *outPropertyData) const {
    const auto result = AudioFileGetProperty(audioFile_, inPropertyID, &ioDataSize, outPropertyData);
    ThrowIfAudioFileError(result, "AudioFileGetProperty");
}

void CXXAudioToolbox::CAAudioFile::SetProperty(AudioFilePropertyID inPropertyID, UInt32 inDataSize,
                                               const void *inPropertyData) {
    const auto result = AudioFileSetProperty(audioFile_, inPropertyID, inDataSize, inPropertyData);
    ThrowIfAudioFileError(result, "AudioFileSetProperty");
}

AudioFileTypeID CXXAudioToolbox::CAAudioFile::FileFormat() const {
    AudioFileTypeID fileFormat;
    UInt32 size = sizeof fileFormat;
    GetProperty(kAudioFilePropertyFileFormat, size, &fileFormat);
    return fileFormat;
}

CXXCoreAudio::CAStreamDescription CXXAudioToolbox::CAAudioFile::DataFormat() const {
    CXXCoreAudio::CAStreamDescription fileDataFormat;
    UInt32 size = sizeof fileDataFormat;
    GetProperty(kAudioFilePropertyDataFormat, size, &fileDataFormat);
    return fileDataFormat;
}

// MARK: Global Properties

UInt32 CXXAudioToolbox::CAAudioFile::GetGlobalInfoSize(AudioFilePropertyID inPropertyID, UInt32 inSpecifierSize,
                                                       void *_Nullable inSpecifier) {
    UInt32 size;
    const auto result = AudioFileGetGlobalInfoSize(inPropertyID, inSpecifierSize, inSpecifier, &size);
    ThrowIfAudioFileError(result, "AudioFileGetGlobalInfoSize");
    return size;
}

void CXXAudioToolbox::CAAudioFile::GetGlobalInfo(AudioFilePropertyID inPropertyID, UInt32 inSpecifierSize,
                                                 void *_Nullable inSpecifier, UInt32& ioDataSize,
                                                 void *outPropertyData) {
    const auto result =
          AudioFileGetGlobalInfo(inPropertyID, inSpecifierSize, inSpecifier, &ioDataSize, outPropertyData);
    ThrowIfAudioFileError(result, "AudioFileGetGlobalInfo");
}

std::vector<AudioFileTypeID> CXXAudioToolbox::CAAudioFile::ReadableTypes() {
    auto size = GetGlobalInfoSize(kAudioFileGlobalInfo_ReadableTypes, 0, nullptr);
    auto count = size / sizeof(AudioFileTypeID);
    auto types = std::vector<AudioFileTypeID>(count);
    GetGlobalInfo(kAudioFileGlobalInfo_ReadableTypes, 0, nullptr, size, types.data());
    return types;
}

std::vector<AudioFileTypeID> CXXAudioToolbox::CAAudioFile::WritableTypes() {
    auto size = GetGlobalInfoSize(kAudioFileGlobalInfo_WritableTypes, 0, nullptr);
    auto count = size / sizeof(AudioFileTypeID);
    auto types = std::vector<AudioFileTypeID>(count);
    GetGlobalInfo(kAudioFileGlobalInfo_WritableTypes, 0, nullptr, size, types.data());
    return types;
}

CFStringRef CXXAudioToolbox::CAAudioFile::CopyFileTypeName(AudioFileTypeID type) {
    CFStringRef s;
    UInt32 size = sizeof s;
    GetGlobalInfo(kAudioFileGlobalInfo_FileTypeName, sizeof type, &type, size, &s);
    return s;
}

std::vector<CXXCoreAudio::CAStreamDescription>
CXXAudioToolbox::CAAudioFile::AvailableStreamDescriptions(AudioFileTypeID fileType, AudioFormatID formatID) {
    AudioFileTypeAndFormatID spec{fileType, formatID};
    auto size = GetGlobalInfoSize(kAudioFileGlobalInfo_AvailableStreamDescriptionsForFormat, sizeof spec, &spec);
    auto count = size / sizeof(AudioStreamBasicDescription);
    auto streamDescriptions = std::vector<CXXCoreAudio::CAStreamDescription>(count);
    GetGlobalInfo(kAudioFileGlobalInfo_AvailableStreamDescriptionsForFormat, sizeof spec, &spec, size,
                  streamDescriptions.data());
    return streamDescriptions;
}

std::vector<AudioFormatID> CXXAudioToolbox::CAAudioFile::AvailableFormatIDs(AudioFileTypeID type) {
    auto size = GetGlobalInfoSize(kAudioFileGlobalInfo_AvailableFormatIDs, sizeof type, &type);
    auto count = size / sizeof(AudioFormatID);
    auto formatIDs = std::vector<AudioFormatID>(count);
    GetGlobalInfo(kAudioFileGlobalInfo_AvailableFormatIDs, sizeof type, &type, size, formatIDs.data());
    return formatIDs;
}

CFArrayRef CXXAudioToolbox::CAAudioFile::CopyAllExtensions() {
    CFArrayRef a;
    UInt32 size = sizeof a;
    GetGlobalInfo(kAudioFileGlobalInfo_AllExtensions, 0, nullptr, size, &a);
    return a;
}

CFArrayRef CXXAudioToolbox::CAAudioFile::CopyAllUTIs() {
    CFArrayRef a;
    UInt32 size = sizeof a;
    GetGlobalInfo(kAudioFileGlobalInfo_AllUTIs, 0, nullptr, size, &a);
    return a;
}

CFArrayRef CXXAudioToolbox::CAAudioFile::CopyAllMIMETypes() {
    CFArrayRef a;
    UInt32 size = sizeof a;
    GetGlobalInfo(kAudioFileGlobalInfo_AllMIMETypes, 0, nullptr, size, &a);
    return a;
}

CFArrayRef CXXAudioToolbox::CAAudioFile::CopyExtensionsForType(AudioFileTypeID type) {
    CFArrayRef a;
    UInt32 size = sizeof a;
    GetGlobalInfo(kAudioFileGlobalInfo_ExtensionsForType, sizeof type, &type, size, &a);
    return a;
}

CFArrayRef CXXAudioToolbox::CAAudioFile::CopyUTIsForType(AudioFileTypeID type) {
    CFArrayRef a;
    UInt32 size = sizeof a;
    GetGlobalInfo(kAudioFileGlobalInfo_UTIsForType, sizeof type, &type, size, &a);
    return a;
}

CFArrayRef CXXAudioToolbox::CAAudioFile::CopyMIMETypesForType(AudioFileTypeID type) {
    CFArrayRef mimeTypes;
    UInt32 size = sizeof mimeTypes;
    GetGlobalInfo(kAudioFileGlobalInfo_MIMETypesForType, sizeof type, &type, size, &mimeTypes);
    return mimeTypes;
}

std::vector<AudioFileTypeID> CXXAudioToolbox::CAAudioFile::TypesForMIMEType(CFStringRef mimeType) {
    auto size = GetGlobalInfoSize(kAudioFileGlobalInfo_TypesForMIMEType, sizeof mimeType,
                                  const_cast<void *>(reinterpret_cast<const void *>(mimeType)));
    auto count = size / sizeof(AudioFileTypeID);
    auto types = std::vector<AudioFileTypeID>(count);
    GetGlobalInfo(kAudioFileGlobalInfo_TypesForMIMEType, sizeof mimeType,
                  const_cast<void *>(reinterpret_cast<const void *>(mimeType)), size, types.data());
    return types;
}

std::vector<AudioFileTypeID> CXXAudioToolbox::CAAudioFile::TypesForUTI(CFStringRef uti) {
    auto size = GetGlobalInfoSize(kAudioFileGlobalInfo_TypesForUTI, sizeof uti,
                                  const_cast<void *>(reinterpret_cast<const void *>(uti)));
    auto count = size / sizeof(AudioFileTypeID);
    auto types = std::vector<AudioFileTypeID>(count);
    GetGlobalInfo(kAudioFileGlobalInfo_TypesForUTI, sizeof uti, const_cast<void *>(reinterpret_cast<const void *>(uti)),
                  size, types.data());
    return types;
}

std::vector<AudioFileTypeID> CXXAudioToolbox::CAAudioFile::TypesForExtension(CFStringRef extension) {
    auto size = GetGlobalInfoSize(kAudioFileGlobalInfo_TypesForExtension, sizeof extension,
                                  const_cast<void *>(reinterpret_cast<const void *>(extension)));
    auto count = size / sizeof(AudioFileTypeID);
    auto types = std::vector<AudioFileTypeID>(count);
    GetGlobalInfo(kAudioFileGlobalInfo_TypesForExtension, sizeof extension,
                  const_cast<void *>(reinterpret_cast<const void *>(extension)), size, types.data());
    return types;
}
