#!/bin/bash

LIBPNG_VER=1.6.29
DOWNLOAD=download
BUILD=build
DEST=`pwd`/..

if [ -d ${DEST}/include/ios/libpng ]; then exit; fi

download() {
    # Download libpng: http://www.libpng.org/pub/png/libpng.html
    if [ ! -f ${DOWNLOAD}/libpng-${LIBPNG_VER}.tar.gz ]; then
      mkdir -p ${DOWNLOAD}
      curl -L http://prdownloads.sourceforge.net/libpng/libpng-${LIBPNG_VER}.tar.gz?download \
           -o ${DOWNLOAD}/libpng-${LIBPNG_VER}.tar.gz
    fi
}
build_ios() {
 	# extract zip
  	mkdir -p ${BUILD}
  	rm -rf ${BUILD}/libpng-${LIBPNG_VER}
  	tar xzf ${DOWNLOAD}/libpng-${LIBPNG_VER}.tar.gz -C ${BUILD}
  	(
  		cd ${BUILD}/libpng-${LIBPNG_VER}
    	# build simulator
    	./configure --prefix=`pwd`/lib/i386 --enable-static=yes --enable-shared=no \
    			CFLAGS="-Ofast -mios-version-min=5.0" LDFLAGS="-flto" CC="xcrun -sdk iphonesimulator clang -arch i386" --host=arm
    	make clean && make -j8 && make install
    	./configure --prefix=`pwd`/lib/x86_64 --enable-static=yes --enable-shared=no \
    			CFLAGS="-Ofast -mios-version-min=5.0" LDFLAGS="-flto" CC="xcrun -sdk iphonesimulator clang -arch x86_64" --host=arm
    	make clean && make -j8 && make install
	    # build armv7
    	./configure --prefix=`pwd`/lib/armv7 --enable-static=yes --enable-shared=no \
    			--enable-arm-neon=api \
    			CFLAGS="-Ofast -mios-version-min=6.0" LDFLAGS="-flto" CC="xcrun -sdk iphoneos clang -arch armv7" --host=arm
    	make clean && make -j8 && make install
    	# build armv7s
    	./configure --prefix=`pwd`/lib/armv7s --enable-static=yes --enable-shared=no \
    			--enable-arm-neon=api \
    			CFLAGS="-Ofast -mios-version-min=6.0" LDFLAGS="-flto" CC="xcrun -sdk iphoneos clang -arch armv7s" --host=arm
	    make clean && make -j8 && make install
    	# build arm64
    	./configure --prefix=`pwd`/lib/iphone --enable-static=yes --enable-shared=no \
    			--enable-arm-neon=api \
    			CFLAGS="-Ofast -mios-version-min=5.0" LDFLAGS="-flto" CC="xcrun --sdk iphoneos clang -arch arm64 " --host=arm
    	make clean && make -j8 && make install

	    mkdir -p ${DEST}/include/ios/libpng/ ${DEST}/lib/ios
	    cp -rf *.h ${DEST}/include/ios/libpng/
	    xcrun lipo -create $(find lib/*/lib -name "libpng.a") -o ${DEST}/lib/ios/libpng.a
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
build_ios

echo [ ] Installed libpng-${LIBPNG_VER} [ios]


