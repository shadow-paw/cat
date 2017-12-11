#!/bin/bash

LIBPNG_VER=1.6.29
DOWNLOAD=download
BUILD=build
DEST=`pwd`/..

if [ -d ${DEST}/include/win32/libpng ]; then exit; fi

VCVARS="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat"
DEVENV="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\IDE\devenv.exe"
MSBUILD="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\Bin\msbuild.exe"

download() {
    # Download libpng: http://www.libpng.org/pub/png/libpng.html
    if [ ! -f ${DOWNLOAD}/libpng-${LIBPNG_VER}.tar.gz ]; then
        mkdir -p ${DOWNLOAD}
        curl -L http://prdownloads.sourceforge.net/libpng/libpng-${LIBPNG_VER}.tar.gz?download \
             -o ${DOWNLOAD}/libpng-${LIBPNG_VER}.tar.gz
  fi
}
build_win() {
    # extract zip
    mkdir -p ${BUILD}
    rm -rf ${BUILD}/libpng-${LIBPNG_VER}
    tar xzf ${DOWNLOAD}/libpng-${LIBPNG_VER}.tar.gz -C ${BUILD}
  
    # Patch with our Makefile and config
    cp -f res/pnglibconf.h res/libpng.vcxproj res/libpng.vcxproj.filters ${BUILD}/libpng-${LIBPNG_VER}/
    "${VCVARS}" "x86"
    "${MSBUILD}" ${BUILD}/libpng-${LIBPNG_VER}/libpng.vcxproj /property:Configuration=Debug /property:Platform=x86
    "${MSBUILD}" ${BUILD}/libpng-${LIBPNG_VER}/libpng.vcxproj /property:Configuration=Release /property:Platform=x86
    "${VCVARS}" "x64"
    "${MSBUILD}" ${BUILD}/libpng-${LIBPNG_VER}/libpng.vcxproj /property:Configuration=Debug /property:Platform=x64
    "${MSBUILD}" ${BUILD}/libpng-${LIBPNG_VER}/libpng.vcxproj /property:Configuration=Release /property:Platform=x64
    for ARCH in win32 win32d win64 win64d
    do
        mkdir -p ${DEST}/include/${ARCH}/libpng/ ${DEST}/lib/${ARCH}/
        cp -f ${BUILD}/libpng-${LIBPNG_VER}/*.h ${DEST}/include/${ARCH}/libpng/
        cp -f ${BUILD}/libpng-${LIBPNG_VER}/lib/${ARCH}/libpng.lib \
              ${BUILD}/libpng-${LIBPNG_VER}/lib/${ARCH}/libpng.pdb \
              ${DEST}/lib/${ARCH}/
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
build_win

echo [ ] Installed libpng-${LIBPNG_VER} [win]
