# C++ Application Tool
> Write your program once with C++, and create Windows, Mac, iOS and Android app.

[![C++ Version][cpp-image]][cpp-url]
[![OpenGL Version][ogl-image]][ogl-url]
[![TravisCI][travis-image]][travis-url]

**CAT** provides an abstract and consistent environment for application development. The main features include:
- Graphics Renderer
- UI Framework
- Time Service
- Network Service
- Audio Service
- Media Service - TODO (camera, video player)
- Sensor Service - TODO

## Examples
- [Blank skeleton](example/blank/src/bootapp.cpp) - program structure and life cycle
- [Basic UI](example/ui_basic/src/bootapp.cpp) - basic ui like pane and button
- [Storage](example/storage/src/bootapp.cpp) - persistent storage
- [Timer](example/timer/src/bootapp.cpp) - create and remove timers
- [HTTP](example/http/src/bootapp.cpp) - rest compatible http client
- [Sound](example/sound/src/pane1.cpp) - audio playback

## Build Environments
You can build app on two environments, sadly you cannot build binaries for all platforms with a single environment. Here is the matrix:

#### Windows Environment
- Windows 32-bit App
- Windows 64-bit App
- Android App

#### Mac Environment
- Mac OS X App
- iOS App
- Android App

## Windows Environment
#### Tools
- [Visual Studio Community 2017][visualstudio-url]
- [Android Studio 3.0][android-url]
- [Git Bash][git-url]

#### Command Line Environment
```
export ANDROID_HOME=c:/android/sdk
export ANDROID_NDK_HOME=c:/android/sdk/ndk-bundle
```
<sup>Change the above path if needed</sup>

#### Setup dependency
```
cd dependency/setup
./glm.sh
./json.sh
./glew-win.sh
./zlib-win.sh
./libpng-win.sh
./libpng-android.sh
```

#### Build libcat - windows
Open `libcat/proj/libcat.vcxproj` with Visual Studio. There are four targets available: `win32`, `win32d`, `win64`, `win64d`. Libraries will be created under `libcat/lib/`.

#### Build example - windows
Open `example/{project}/proj/example.vcxproj` with Visual Studio. There are four targets available: `win32`, `win32d`, `win64`, `win64d`. Excutables will be created under `example/{project}/bin/`.

#### Build libcat - android
```
cd libcat/proj/
${ANDROID_NDK_HOME}/ndk-build.cmd -j4
```
  
#### Build example - android
Open `example/{project}/proj/android` with Android Studio. APK will be created under `example/{project}/bin/android/`.

## Mac Environment
#### Tools
- [Xcode 9][xcode-url] with command line tools
- [Android Studio 3.0][android-url]

#### Xcode command line tools
```
xcode-select --install
```

#### Command Line Environment
```
export ANDROID_HOME=/usr/local/android
export ANDROID_NDK_HOME=/usr/local/android/ndk-bundle
```
<sup>Change the above path if needed</sup>

#### Setup dependency
```
cd dependency/setup
./glm.sh
./json.sh
./libpng-mac.sh
./libpng-ios.sh
./libpng-android.sh
```

#### Build libcat - mac & ios
Open `libcat/proj/libcat.xcodeproj` with Xcode. Edit scheme and choose ``Release`` under ``Run`` section. There are two targets available: `mac`, `ios`. Libraries will be created under `libcat/lib/`.

#### Build example - mac & ios
Open `example/{project}/proj/example.xcodeproj` with Xcode. Edit scheme and choose ``Release`` under ``Run`` section. There are two targets available: `mac`, `ios`. Excutables will be created under `example/{project}/bin/`.

#### Build libcat - android
```
cd libcat/proj/
${ANDROID_NDK_HOME}/ndk-build -j4
```

#### Build example - android
Open `example/{project}/proj/android` with Android Studio. APK will be created under `example/{project}/bin/android/`.

<sup>Note: For [Android Emulator](https://developer.android.com/studio/releases/emulator.html) on Mac, OpenGL3 is only available to system image with API 24 or above, note that you can still target at API 18.</sup>

<sup>Note: To build windows app on mac, you need to bootcamp or VM into Windows.</sup>

## License

Distributed under the MIT license. See ``LICENSE`` for more information.

## Contributing

1. Fork it (<https://github.com/shadow-paw/cat/fork>)
2. Create your feature branch (`git checkout origin/master -b feature-foobar`)
3. Commit your changes (`git commit -am 'Add some foobar'`)
4. Push to the branch (`git push origin feature-foobar:feature-foobar`)
5. Create a new Pull Request

<!-- Markdown link & img dfn's -->
[cpp-image]: https://img.shields.io/badge/c%2B%2B-14-blue.svg
[cpp-url]: https://en.wikipedia.org/wiki/C%2B%2B14
[ogl-image]: https://img.shields.io/badge/opengl-3.3%20%7C%203.0%20es-blue.svg
[ogl-url]: https://en.wikipedia.org/wiki/OpenGL
[travis-image]: https://travis-ci.org/shadow-paw/cat.svg?branch=master
[travis-url]: https://travis-ci.org/shadow-paw/cat
[visualstudio-url]: https://www.visualstudio.com/downloads/
[xcode-url]: https://developer.apple.com/xcode/
[android-url]: https://developer.android.com/studio/index.html
[git-url]: https://git-scm.com/downloads
