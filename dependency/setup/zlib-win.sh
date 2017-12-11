#!/bin/bash

ZLIB_VER=1.2.11
DOWNLOAD=download
BUILD=build
DEST=..

if [ -d ${DEST}/include/win32/zlib ]; then exit; fi

VCVARS="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat"
DEVENV="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\IDE\devenv.exe"
MSBUILD="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\Bin\msbuild.exe"

if [ -d ${DEST}/include/win32/zlib ]; then exit; fi

download() {
    # Download zlib: https://zlib.net/
    if [ ! -f ${DOWNLOAD}/zlib-${ZLIB_VER}.tar.gz ]; then
        mkdir -p ${DOWNLOAD}
        curl -L https://zlib.net/zlib-${ZLIB_VER}.tar.gz -o ${DOWNLOAD}/zlib-${ZLIB_VER}.tar.gz
    fi
}
build_win() {
    # extract zip
    mkdir -p ${BUILD}
    rm -rf ${BUILD}/zlib-${ZLIB_VER}
    tar xzf ${DOWNLOAD}/zlib-${ZLIB_VER}.tar.gz -C ${BUILD}
  
    # Patch
    cp -f res/zlib.vcxproj res/zlib.vcxproj.filters ${BUILD}/zlib-${ZLIB_VER}/

    "${VCVARS}" "x86"
    "${MSBUILD}" ${BUILD}/zlib-${ZLIB_VER}/zlib.vcxproj /property:Configuration=Debug /property:Platform=x86
    "${MSBUILD}" ${BUILD}/zlib-${ZLIB_VER}/zlib.vcxproj /property:Configuration=Release /property:Platform=x86
    "${VCVARS}" "x64"
    "${MSBUILD}" ${BUILD}/zlib-${ZLIB_VER}/zlib.vcxproj /property:Configuration=Debug /property:Platform=x64
    "${MSBUILD}" ${BUILD}/zlib-${ZLIB_VER}/zlib.vcxproj /property:Configuration=Release /property:Platform=x64

    for ARCH in win32 win32d win64 win64d
    do
        mkdir -p ${DEST}/include/${ARCH}/zlib/ ${DEST}/lib/${ARCH}/
        cp -f ${BUILD}/zlib-${ZLIB_VER}/zlib.h \
              ${BUILD}/zlib-${ZLIB_VER}/zconf.h \
              ${DEST}/include/${ARCH}/zlib/
        cp -f ${BUILD}/zlib-${ZLIB_VER}/lib/${ARCH}/zlib.dll \
              ${BUILD}/zlib-${ZLIB_VER}/lib/${ARCH}/zlib.lib \
              ${BUILD}/zlib-${ZLIB_VER}/lib/${ARCH}/zlib.pdb \
              ${DEST}/lib/${ARCH}/
    done
    # license
    if [ ! -f ${DEST}/license/zlib.txt ]; then
        mkdir -p ${DEST}/license
        cp -f ${BUILD}/zlib-${ZLIB_VER}/README ${DEST}/license/zlib.txt
    fi
}

download
build_win

echo [ ] Installed zlib-${ZLIB_VER} [win]
