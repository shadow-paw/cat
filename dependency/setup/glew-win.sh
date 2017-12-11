#!/bin/sh
DOWNLOAD=download
BUILD=build
DEST=`pwd`/..

if [ -d ${DEST}/include/win32/glew ]; then exit; fi

download() {
    if [ ! -f ${DOWNLOAD}/glew-2.0.0-win32.zip ]; then
        mkdir -p ${DOWNLOAD}
        curl -L https://sourceforge.net/projects/glew/files/glew/2.0.0/glew-2.0.0-win32.zip/download -o ${DOWNLOAD}/glew-2.0.0-win32.zip
    fi
}
install_win() {
    # extract zip
    mkdir -p ${BUILD}
    rm -rf ${BUILD}/glew-2.0.0
    unzip ${DOWNLOAD}/glew-2.0.0-win32.zip -d ${BUILD}

    # header
    mkdir -p ${DEST}/include/win32/glew ${DEST}/include/win32d/glew ${DEST}/include/win64/glew ${DEST}/include/win64d/glew
    cp -f ${BUILD}/glew-2.0.0/include/GL/*.h ${DEST}/include/win32/glew/
    cp -f ${BUILD}/glew-2.0.0/include/GL/*.h ${DEST}/include/win32d/glew/
    cp -f ${BUILD}/glew-2.0.0/include/GL/*.h ${DEST}/include/win64/glew/
    cp -f ${BUILD}/glew-2.0.0/include/GL/*.h ${DEST}/include/win64d/glew/
    # library
    mkdir -p ${DEST}/lib/win32 ${DEST}/lib/win32d ${DEST}/lib/win64 ${DEST}/lib/win64d
    cp -f ${BUILD}/glew-2.0.0/bin/Release/win32/*.dll ${DEST}/lib/win32
    cp -f ${BUILD}/glew-2.0.0/lib/Release/win32/*.lib ${DEST}/lib/win32
    cp -f ${BUILD}/glew-2.0.0/bin/Release/win32/*.dll ${DEST}/lib/win32d
    cp -f ${BUILD}/glew-2.0.0/lib/Release/win32/*.lib ${DEST}/lib/win32d
    cp -f ${BUILD}/glew-2.0.0/bin/Release/x64/*.dll ${DEST}/lib/win64
    cp -f ${BUILD}/glew-2.0.0/lib/Release/x64/*.lib ${DEST}/lib/win64
    cp -f ${BUILD}/glew-2.0.0/bin/Release/x64/*.dll ${DEST}/lib/win64d
    cp -f ${BUILD}/glew-2.0.0/lib/Release/x64/*.lib ${DEST}/lib/win64d
    #license
    if [ ! -f ${DEST}/license/glew.txt ]; then
        mkdir -p ${DEST}/license
        cp -f ${BUILD}/glew-2.0.0/LICENSE.txt ${DEST}/license/glew.txt
    fi
}

download
install_win

echo [ ] Installed glew [win]
