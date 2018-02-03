#!/bin/sh

DOWNLOAD=download
DEST=`pwd`/..

if [ -d ${DEST}/include/generic/nlohmann ]; then exit; fi

if [ ! -d ${DOWNLOAD}/nlohmann_json ]; then
    mkdir -p ${DOWNLOAD}
    git clone https://github.com/nlohmann/json ${DOWNLOAD}/nlohmann_json
fi

# license
if [ ! -f ${DEST}/license/nlohmann_json.txt ]; then
    mkdir -p ${DEST}/license
    cp -f  ${DOWNLOAD}/nlohmann_json/LICENSE.MIT ${DEST}/license/nlohmann_json.txt
fi

mkdir -p ${DEST}/include/generic
cp -rf ${DOWNLOAD}/nlohmann_json/single_include/nlohmann ${DEST}/include/generic/

echo [ ] Installed nlohmann-json
