# Operating System Abstraction Layer
> Write your program once with C++, and deliver Windows, Mac, iOS and Android app.

![Platform][platform-image]
[![C++ Version][cpp-image]][cpp-url]
[![License][license-image]][license-url]

The OSAL project provide an abstracted and consistent environment for application devlopment. The main features include:
- Graphics rendering with OpenGL / OpenGL ES.
- UI Framework
- Network Access
- Camera and Sensor Access (if available)

## Build Environments
You can build osal app on two environments, sadly you cannot build binaries for all platform with a single environment. Here is the matrix:

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
export ANDROID_SDK=c:/android/sdk
export ANDROID_NDK=c:/android/sdk/ndk-bundle
```

#### Setup dependency
```
cd dependency/setup
./glm.sh
./glew-win.sh
./zlib-win.sh
./libpng-win.sh
./libpng-android.sh
```

#### Build libosal - windows
Open `libosal/proj/libosal.vcxproj` with Visual Studio. There are four targets available: `win32`, `win32d`, `win64`, `win64d`. Libraries will be created under `libosal/lib/`.

#### Build example - windows
Open `example/hello/proj/example.vcxproj` with Visual Studio. There are four targets available: `win32`, `win32d`, `win64`, `win64d`. Excutables will be created under `example/hello/bin/`.

#### Build libosal - android
```
cd libosal/proj/
ndk-build.cmd -j4
```

#### Build example - android
Open `example/hello/proj/android` with Android Studio. You can adjust manifest and stuffs in `example/hello/glue/android`. APK will be created under `example/hello/bin/android/`.

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
export ANDROID_SDK=/usr/local/android
export ANDROID_NDK=/usr/local/android/ndk-bundle
```

#### Setup dependency
```
cd dependency/setup
./glm.sh
./libpng-mac.sh
./libpng-ios.sh
./libpng-android.sh
```

#### Build libosal - mac & ios
Open `libosal/proj/libosal.xcodeproj` with Xcode. There are two targets available: `mac`, `ios`. Libraries will be created under `libosal/lib/`.

#### Build example - windows
Open `example/hello/proj/example.xcodeproj` with Xcode. There are two targets available: `mac`, `ios`. Excutables will be created under `example/hello/bin/`.

#### Build libosal - android
```
cd libosal/proj/
ndk-build -j4
```

#### Build example - android
Open `example/hello/proj/android` with Android Studio. You can adjust manifest and stuffs in `example/hello/glue/android`. APK will be created under `example/hello/bin/android/`.

<sup>Note: To build windows app on mac, you need to bootcamp or VM into Windows.</sup>

## License

Distributed under the MIT license. See ``LICENSE`` for more information.

## Contributing

1. Fork it (<https://github.com/shadow-paw/osal/fork>)
2. Create your feature branch (`git checkout origin/master -b feature-foobar`)
3. Commit your changes (`git commit -am 'Add some foobar'`)
4. Push to the branch (`git push origin feature-foobar:feature-foobar`)
5. Create a new Pull Request

<!-- Markdown link & img dfn's -->
[platform-image]: https://img.shields.io/badge/platform-win32%20%7C%20win64%20%7C%20mac%20%7C%20ios%20%7C%20android-lightgrey.svg
[cpp-image]: https://img.shields.io/badge/c%2B%2B-14-green.svg
[cpp-url]: https://en.wikipedia.org/wiki/C%2B%2B14
[license-image]: https://img.shields.io/badge/license-MIT-blue.svg
[license-url]: LICENSE
[visualstudio-url]: https://www.visualstudio.com/downloads/
[xcode-url]: https://developer.apple.com/xcode/
[android-url]: https://developer.android.com/studio/preview/index.html
[git-url]: https://git-scm.com/downloads