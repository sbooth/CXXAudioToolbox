# CXXAudioToolbox

Assorted C++ classes simplifying the use of Audio Toolbox.

| Class | Description |
| --- | --- |
| [CAAudioConverter](Sources/CXXAudioToolbox/include/CXXAudioToolbox/CAAudioConverter.hpp) | An [`AudioConverter`](https://developer.apple.com/documentation/audiotoolbox/audio-converter-services?language=objc) wrapper.|
| [CAAudioFile](Sources/CXXAudioToolbox/include/CXXAudioToolbox/CAAudioFile.hpp) | An [`AudioFile`](https://developer.apple.com/documentation/audiotoolbox/audio-file-services?language=objc) wrapper. |
| [CAAudioFormat](Sources/CXXAudioToolbox/include/CXXAudioToolbox/CAAudioFormat.hpp) | An [`AudioFormat`](https://developer.apple.com/documentation/audiotoolbox/audio-format-services?language=objc) wrapper. |
| [CAAUGraph](Sources/CXXAudioToolbox/include/CXXAudioToolbox/CAAUGraph.hpp) | An [`AUGraph`](https://developer.apple.com/documentation/audiotoolbox/audio-unit-processing-graph-services?language=objc) wrapper. |
| [CAExtAudioFile](Sources/CXXAudioToolbox/include/CXXAudioToolbox/CAExtAudioFile.hpp) | An [`ExtAudioFile`](https://developer.apple.com/documentation/audiotoolbox/extended-audio-file-services?language=objc) wrapper. |
| [AudioFileWrapper](Sources/CXXAudioToolbox/include/CXXAudioToolbox/AudioFileWrapper.hpp) | A bare-bones [`AudioFile`](https://developer.apple.com/documentation/audiotoolbox/audio-file-services?language=objc) wrapper modeled after [`std::unique_ptr`](https://en.cppreference.com/w/cpp/memory/unique_ptr.html). |
| [ExtAudioFileWrapper](Sources/CXXAudioToolbox/include/CXXAudioToolbox/ExtAudioFileWrapper.hpp) | A bare-bones [`ExtAudioFile`](https://developer.apple.com/documentation/audiotoolbox/extended-audio-file-services?language=objc) wrapper modeled after [`std::unique_ptr`](https://en.cppreference.com/w/cpp/memory/unique_ptr.html). |

> [!NOTE]
> C++17 is required.

All classes are in the `CXXAudioToolbox` namespace.

This package depends on the [CXXCoreAudio](https://github.com/sbooth/CXXCoreAudio) package.

## Installation

### Swift Package Manager

Add a package dependency to https://github.com/sbooth/CXXAudioToolbox in Xcode.

### Manual or Custom Build

1. Clone the [CXXAudioToolbox](https://github.com/sbooth/CXXAudioToolbox) repository.
2. `swift build`.

## License

Released under the [MIT License](https://github.com/sbooth/CXXAudioToolbox/blob/main/LICENSE.txt).
