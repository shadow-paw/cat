#!/bin/sh

DOWNLOAD=download
DEST=`pwd`/..

if [ -d ${DEST}/include/generic/glm ]; then exit; fi

if [ ! -d ${DOWNLOAD}/glm ]; then
    mkdir -p ${DOWNLOAD}
    # curl -L https://github.com/g-truc/glm/archive/${GLM_VER}.tar.gz -o ${DOWNLOAD}/glm-${GLM_VER}.tar.gz
    git clone https://github.com/g-truc/glm.git ${DOWNLOAD}/glm
fi

mkdir -p ${DEST}/include/generic
cp -rf ${DOWNLOAD}/glm/glm ${DEST}/include/generic/

echo [ ] Installed glm
