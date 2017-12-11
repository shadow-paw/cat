#!/bin/bash

LIBPNG_VER=1.6.29
DOWNLOAD=download
BUILD=build
DEST=`pwd`/..

if [ -d ${DEST}/include/mac/libpng ]; then exit; fi

download() {
    # Download libpng: http://www.libpng.org/pub/png/libpng.html
    if [ ! -f ${DOWNLOAD}/libpng-${LIBPNG_VER}.tar.gz ]; then
		mkdir -p ${DOWNLOAD}
      	curl -L http://prdownloads.sourceforge.net/libpng/libpng-${LIBPNG_VER}.tar.gz?download \
             -o ${DOWNLOAD}/libpng-${LIBPNG_VER}.tar.gz
    fi
}
build_mac() {
	# extract zip
  	mkdir -p ${BUILD}
  	rm -rf ${BUILD}/libpng-${LIBPNG_VER}
  	tar xzf ${DOWNLOAD}/libpng-${LIBPNG_VER}.tar.gz -C ${BUILD}
  	(
  		cd ${BUILD}/libpng-${LIBPNG_VER}
    	# build macosx
    	./configure --prefix=`pwd`/lib/mac --enable-static=yes --enable-shared=no \
    			CFLAGS="-Ofast -mmacosx-version-min=10.7" LDFLAGS="-flto" CC="xcrun -sdk macosx clang -arch x86_64"
    	make clean && make -j8 && make install
    	# install_name_tool -id "@executable_path/../Frameworkds/libpng.dylib" lib/mac/lib/libpng.dylib

	    mkdir -p ${DEST}/include/mac/libpng/ ${DEST}/lib/mac/
    	cp -rf *.h ${DEST}/include/mac/libpng/
    	#LIB=libpng.dylib; while readlink lib/mac/lib/$LIB > /dev/null; do LIB=$(readlink lib/mac/lib/$LIB); done;
    	#cp lib/mac/lib/$LIB ${DEST}/lib/mac/libpng.dylib
    	cp lib/mac/lib/libpng.a ${DEST}/lib/mac/
  	)
    # license
    if [ ! -f ${DEST}/license/libpng.txt ]; then
        mkdir -p ${DEST}/license
        cp -f ${BUILD}/libpng-${LIBPNG_VER}/LICENSE ${DEST}/license/libpng.txt
    fi
    # cleanup
	rm -rf ${BUILD}/libpng-${LIBPNG_VER}
}

download
build_mac

echo [ ] Installed libpng-${LIBPNG_VER} [mac]


