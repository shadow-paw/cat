# C++ Application Tool
> Write your program once with C++, and create Windows, Mac, iOS and Android app.

![win](https://img.shields.io/badge/win-pass-brightgreen.svg)
![mac](https://img.shields.io/badge/mac-pass-brightgreen.svg)
![ios](https://img.shields.io/badge/ios-pass-brightgreen.svg)
![android](https://img.shields.io/badge/android-pass-brightgreen.svg)
[![C++ Version][cpp-image]][cpp-url]
[![License][license-image]][license-url]

**CAT** provides an abstract and consistent environment for application development. The main features include:
- Graphics Rendering Service
- UI Framework
- Time Service
- Network Service
- Audio Service
- Media Service (camera, mic, media player)
- Sensor Service

## Examples
- [Blank skeleton](example/blank/src/bootapp.cpp)
- [Builtin UI](example/ui_builtin/src/bootapp.cpp)
- [Custom UI](example/ui_custom/src/bootapp.cpp)
- [Timer](example/timer/src/bootapp.cpp)
```
kernel()->time()->post_timer(this, 1, 1000);
kernel()->time()->remove_timer(this, 1);
```
- [HTTP](example/http/src/bootapp.cpp)
```
HttpRequest req("https://httpbin.org/post");
req.add_header("foo", "bar");
req.add_header("foo2", "dumb");
req.post("Post Data", "text/plain; charset=utf-8");
auto http_id = kernel()->net()->http_fetch(std::move(req), [](const HttpResponse& res) -> void {
    for (auto it = res.headers.begin(); it != res.headers.end(); ++it) {
        Logger::d("App", "http -> header = %s:%s", it->first.c_str(), it->second.c_str());
    }
    Logger::d("App", "http -> %d - %s", res.code, res.body.ptr());
});
```

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

#### Build libcat - windows
Open `libcat/proj/libcat.vcxproj` with Visual Studio. There are four targets available: `win32`, `win32d`, `win64`, `win64d`. Libraries will be created under `libcat/lib/`.

#### Build example - windows
Open `example/{project}/proj/example.vcxproj` with Visual Studio. There are four targets available: `win32`, `win32d`, `win64`, `win64d`. Excutables will be created under `example/{project}/bin/`.

#### Build libcat - android
```
cd libcat/proj/
ndk-build.cmd -j4
```
  
#### Build example - android
Open `example/{project}/proj/android` with Android Studio. You can adjust manifest and stuffs in `example/{project}/glue/android`. APK will be created under `example/{project}/bin/android/`.

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

#### Build libcat - mac & ios
Open `libcat/proj/libcat.xcodeproj` with Xcode. Edit scheme and choose ``Release`` under ``Run`` section. There are two targets available: `mac`, `ios`. Libraries will be created under `libcat/lib/`.

#### Build example - mac & ios
Open `example/{project}/proj/example.xcodeproj` with Xcode. Edit scheme and choose ``Release`` under ``Run`` section. There are two targets available: `mac`, `ios`. Excutables will be created under `example/{project}/bin/`.

#### Build libcat - android
```
cd libcat/proj/
ndk-build -j4
```

#### Build example - android
Open `example/{project}/proj/android` with Android Studio. You can adjust manifest and stuffs in `example/{project}/glue/android`. APK will be created under `example/{project}/bin/android/`.

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
[cpp-image]: https://img.shields.io/badge/c%2B%2B-14-green.svg
[cpp-url]: https://en.wikipedia.org/wiki/C%2B%2B14
[license-image]: https://img.shields.io/badge/license-MIT-blue.svg
[license-url]: LICENSE
[visualstudio-url]: https://www.visualstudio.com/downloads/
[xcode-url]: https://developer.apple.com/xcode/
[android-url]: https://developer.android.com/studio/preview/index.html
[git-url]: https://git-scm.com/downloads
