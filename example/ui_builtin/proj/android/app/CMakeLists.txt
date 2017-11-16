# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)
set(CMAKE_CXX_STANDARD 14)

set (CAT_BASE_PATH ${CMAKE_SOURCE_DIR}/../../../../../)
set (APP_BASE_PATH ${CMAKE_SOURCE_DIR}/../../../)

include_directories(
    ${CAT_BASE_PATH}/dependency/include/generic
    ${CAT_BASE_PATH}/dependency/include/android
    ${CAT_BASE_PATH}/libcat/include
    ${APP_BASE_PATH}/src/
)
add_library(osal-app
            SHARED
            ${APP_BASE_PATH}/glue/android/cpp/jni.cpp
            ${APP_BASE_PATH}/src/bootapp.cpp
            )

find_library(lib-android android)
find_library(lib-log log)
find_library(lib-z z)
find_library(lib-GLESv2 GLESv2)
find_library(lib-EGL EGL)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries(osal-app
                      ${CAT_BASE_PATH}/libcat/lib/android/${ANDROID_ABI}/libcat.a
                      ${CAT_BASE_PATH}/dependency/lib/android/${ANDROID_ABI}/libpng.a
                      ${lib-EGL}
                      ${lib-GLESv2}
                      ${lib-z}
                      ${lib-log}
                      ${lib-android}
                     )