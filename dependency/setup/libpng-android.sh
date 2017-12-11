#!/bin/bash

LIBPNG_VER=1.6.29
DOWNLOAD=download
BUILD=build
DEST=`pwd`/..

if [ -d ${DEST}/include/android/libpng ]; then exit; fi

if [ -z "$ANDROID_NDK_HOME" ]; then
  ANDROID_NDK_HOME=${ANDROID_HOME}/ndk-bundle
fi
if [ -f ${ANDROID_NDK_HOME}/ndk-build.cmd ]; then
  ANDROID_NDK_BUILD=${ANDROID_NDK_HOME}/ndk-build.cmd
else
  ANDROID_NDK_BUILD=${ANDROID_NDK_HOME}/ndk-build
fi

download() {
    # Download libpng: http://www.libpng.org/pub/png/libpng.html
    if [ ! -f ${DOWNLOAD}/libpng-${LIBPNG_VER}.tar.gz ]; then
      mkdir -p ${DOWNLOAD}
      curl -L http://prdownloads.sourceforge.net/libpng/libpng-${LIBPNG_VER}.tar.gz?download \
           -o ${DOWNLOAD}/libpng-${LIBPNG_VER}.tar.gz
    fi
}
build_android() {
    # extract zip
    mkdir -p ${BUILD}
    rm -rf ${BUILD}/libpng-${LIBPNG_VER}
    tar xzf ${DOWNLOAD}/libpng-${LIBPNG_VER}.tar.gz -C ${BUILD}
  
    # Patch with our Makefile and config
    mkdir -p ${BUILD}/libpng-${LIBPNG_VER}/jni
    cp -f res/pnglibconf.h ${BUILD}/libpng-${LIBPNG_VER}/
    cp -f res/libpng-jni/* ${BUILD}/libpng-${LIBPNG_VER}/jni/
    # Build it
    ${ANDROID_NDK_BUILD} -j8 NDK_PROJECT_PATH=${BUILD}/libpng-${LIBPNG_VER} -I..
    # install
    mkdir -p ${DEST}/include/android/libpng
    cp -f ${BUILD}/libpng-${LIBPNG_VER}/*.h ${DEST}/include/android/libpng/
    for ARCH in armeabi armeabi-v7a arm64-v8a mips mips64 x86 x86_64
    do
        mkdir -p ${DEST}/lib/android/${ARCH}/
        cp -f ${BUILD}/libpng-${LIBPNG_VER}/obj/local/${ARCH}/libpng.a ${DEST}/lib/android/${ARCH}/
    done
    # license
    if [ ! -f ${DEST}/license/libpng.txt ]; then
        mkdir -p ${DEST}/license
        cp -f ${BUILD}/libpng-${LIBPNG_VER}/LICENSE ${DEST}/license/libpng.txt
    fi
    # cleanup
    rm -rf ${BUILD}/libpng-${LIBPNG_VER}
}

download
build_android

echo [ ] Installed libpng-${LIBPNG_VER} [android]
