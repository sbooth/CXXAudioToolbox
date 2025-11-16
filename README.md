# CXXAudioToolbox

Assorted C++ classes simplifying the use of Audio Toolbox.

| C++ Class | Description |
| --- | --- |
| [ATAudioConverter](Sources/CXXAudioToolbox/include/CXXAudioToolbox/ATAudioConverter.hpp) | An `AudioConverter` wrapper.|
| [ATAudioFile](Sources/CXXAudioToolbox/include/CXXAudioToolbox/ATAudioFile.hpp) | An `AudioFile` wrapper. |
| [ATAudioFormat](Sources/CXXAudioToolbox/include/CXXAudioToolbox/ATAudioFormat.hpp) | An `AudioFormat` wrapper. |
| [ATAUGraph](Sources/CXXAudioToolbox/include/CXXAudioToolbox/ATAUGraph.hpp) | An `AUGraph` wrapper. |
| [AudioFileWrapper](Sources/CXXAudioToolbox/include/CXXAudioToolbox/SFBAudioFileWrapper.hpp) | A bare-bones `AudioFile` wrapper modeled after [`std::unique_ptr`](https://en.cppreference.com/w/cpp/memory/unique_ptr.html). |
| [ExtAudioFileWrapper](Sources/CXXAudioToolbox/include/CXXAudioToolbox/SFBExtAudioFileWrapper.hpp) | A bare-bones `ExtAudioFile` wrapper modeled after [`std::unique_ptr`](https://en.cppreference.com/w/cpp/memory/unique_ptr.html). |

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
