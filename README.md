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
- [Android Studio 4.0][android-url]
- [Git Bash][git-url]
- [VCPKG][vcpkg-url]

#### Install vcpkg
```
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg/
./bootstrap-vcpkg.sh
./vcpkg integrate install
```
###### Android tweaks
https://github.com/microsoft/vcpkg/blob/master/docs/users/android.md
```
export ANDROID_NDK_HOME=/c/android/sdk/ndk/21.3.6528147
export VCPKG_ROOT=/c/vcpkg
```
Add to windows environment for gradle, right click This PC > Properties > Advanced System Settings > Environment Variables
```
ANDROID_NDK_HOME=c:/android/sdk/ndk/21.3.6528147
VCPKG_ROOT=c:/devtools/vcpkg
```
<sup>Change the above path if needed</sup>

#### Install dependency
```
${VCPKG_ROOT}/vcpkg install glm zlib libpng libjpeg-turbo glew --triplet x86-windows
${VCPKG_ROOT}/vcpkg install glm zlib libpng libjpeg-turbo glew --triplet x64-windows
${VCPKG_ROOT}/vcpkg install glm zlib libpng libjpeg-turbo --triplet arm-android
${VCPKG_ROOT}/vcpkg install glm zlib libpng libjpeg-turbo --triplet arm64-android
${VCPKG_ROOT}/vcpkg install glm zlib libpng libjpeg-turbo --triplet x86-android
${VCPKG_ROOT}/vcpkg install glm zlib libpng libjpeg-turbo --triplet x64-android
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
- [Xcode][xcode-url] with command line tools
- [Android Studio 4.0][android-url]
- [VCPKG][vcpkg-url]
- [CocoaPod][cocoapod-url]

#### Xcode command line tools
```
xcode-select --install
```

#### Install vcpkg
```
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg/
./bootstrap-vcpkg.sh
./vcpkg integrate install
```
###### Android tweaks
https://github.com/microsoft/vcpkg/blob/master/docs/users/android.md
```
export VCPKG_ROOT=$HOME/devel/git/public/vcpkg
export ANDROID_NDK_HOME=$HOME/Library/Android/sdk/ndk/21.3.6528147/
```
<sup>Change the above path if needed</sup>

#### Install dependency
```
${VCPKG_ROOT}/vcpkg install glm zlib libpng libjpeg-turbo --triplet arm-android
${VCPKG_ROOT}/vcpkg install glm zlib libpng libjpeg-turbo --triplet arm64-android
${VCPKG_ROOT}/vcpkg install glm zlib libpng libjpeg-turbo --triplet x86-android
${VCPKG_ROOT}/vcpkg install glm zlib libpng libjpeg-turbo --triplet x64-android
```

#### Build libcat - mac & ios
```
cd libcat/proj
pod install
```
Open `libcat/proj/libcat.xcodeproj` with Xcode. Edit scheme and choose ``Release`` under ``Run`` section. There are two targets available: `mac`, `ios`. Libraries will be created under `libcat/lib/`.

#### Build example - mac & ios
```
cd example/{project}/proj
pod install
```
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
[vcpkg-url]: https://github.com/Microsoft/vcpkg.git
[cocoapod-url]: https://cocoapods.org/
