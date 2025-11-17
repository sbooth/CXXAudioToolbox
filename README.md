# CXXAudioToolbox

Assorted C++ classes simplifying the use of Audio Toolbox.

| C++ Class | Description |
| --- | --- |
| [ATAudioConverter](Sources/CXXAudioToolbox/include/CXXAudioToolbox/ATAudioConverter.hpp) | An [`AudioConverter`](https://developer.apple.com/documentation/audiotoolbox/audio-converter-services?language=objc) wrapper.|
| [ATAudioFile](Sources/CXXAudioToolbox/include/CXXAudioToolbox/ATAudioFile.hpp) | An [`AudioFile`](https://developer.apple.com/documentation/audiotoolbox/audio-file-services?language=objc) wrapper. |
| [ATAudioFormat](Sources/CXXAudioToolbox/include/CXXAudioToolbox/ATAudioFormat.hpp) | An [`AudioFormat`](https://developer.apple.com/documentation/audiotoolbox/audio-format-services?language=objc) wrapper. |
| [ATAUGraph](Sources/CXXAudioToolbox/include/CXXAudioToolbox/ATAUGraph.hpp) | An [`AUGraph`](https://developer.apple.com/documentation/audiotoolbox/audio-unit-processing-graph-services?language=objc) wrapper. |
| [ATExtAudioFile](Sources/CXXAudioToolbox/include/CXXAudioToolbox/ATExtAudioFile.hpp) | An [`ExtAudioFile`](https://developer.apple.com/documentation/audiotoolbox/extended-audio-file-services?language=objc) wrapper. |
| [AudioFileWrapper](Sources/CXXAudioToolbox/include/CXXAudioToolbox/AudioFileWrapper.hpp) | A bare-bones [`AudioFile`](https://developer.apple.com/documentation/audiotoolbox/audio-file-services?language=objc) wrapper modeled after [`std::unique_ptr`](https://en.cppreference.com/w/cpp/memory/unique_ptr.html). |
| [ExtAudioFileWrapper](Sources/CXXAudioToolbox/include/CXXAudioToolbox/ExtAudioFileWrapper.hpp) | A bare-bones [`ExtAudioFile`](https://developer.apple.com/documentation/audiotoolbox/extended-audio-file-services?language=objc) wrapper modeled after [`std::unique_ptr`](https://en.cppreference.com/w/cpp/memory/unique_ptr.html). |

> [!NOTE]
> C++17 is required.

All classes are in the `CXXAudioToolbox` namespace.

## Installation

### Swift Package Manager

Add a package dependency to https://github.com/sbooth/CXXAudioToolbox in Xcode.

### Manual or Custom Build

1. Clone the [CXXAudioToolbox](https://github.com/sbooth/CXXAudioToolbox) repository.
2. `swift build`.

## License

Released under the [MIT License](https://github.com/sbooth/CXXAudioToolbox/blob/main/LICENSE.txt).
