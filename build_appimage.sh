#!/bin/bash
if [ -f libDanSDL2legacy.a ];
then
    rm libDanSDL2legacy.a
fi

if [ -f familiar_temple.out ];
then
    rm familiar_temple.out
fi

cd libdan2
if [ -d objects ];
then
	rm -rf objects
fi

if [ -f libDanSDL2legacy.a ];
then
    rm libDanSDL2legacy.a
fi

mkdir -p objects
make -f makefile_linux -j4
cp ./libDanSDL2legacy.a ../
cd ..

if [ -d objects ];
then
	rm -rf objects
fi;
mkdir -p objects
make -f makefile_linux -j4 BUILDTYPEFLAGS=-DAS_APPIMAGE=1
