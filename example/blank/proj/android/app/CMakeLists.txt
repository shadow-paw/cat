# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

if (VCPKG_TARGET_ANDROID)
    include("vcpkg_android.cmake")
endif()

cmake_minimum_required(VERSION 3.4.1)
set(CMAKE_CXX_STANDARD 14)

set (CAT_BASE_PATH ${CMAKE_SOURCE_DIR}/../../../../../)
set (APP_BASE_PATH ${CMAKE_SOURCE_DIR}/../../../)

include_directories(
    $ENV{VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/include
    ${CAT_BASE_PATH}/libcat/include
    ${APP_BASE_PATH}/src/
)
link_directories(
    $ENV{VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/lib
)

add_library(
    cat-app
    SHARED
    ${CAT_BASE_PATH}/bootstrap/android/cpp/jni.cpp
    ${APP_BASE_PATH}/src/bootapp.cpp
)

find_library(lib-android android)
find_library(lib-log log)
find_library(lib-z z)
find_library(lib-GLESv3 GLESv3)
find_library(lib-EGL EGL)
find_library(lib-SLES OpenSLES)

target_link_libraries(
    cat-app
    ${CAT_BASE_PATH}/libcat/lib/android/${ANDROID_ABI}/libcat.a
    png
    ${lib-SLES}
    ${lib-EGL}
    ${lib-GLESv3}
    ${lib-z}
    ${lib-log}
    ${lib-android}
)

# Copy .so files into apk
file(
    COPY $ENV{VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/lib/libpng16.so
    DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/libs/${ANDROID_ABI}
)             
