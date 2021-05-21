#!/bin/bash
if [[ $# -ne 1 ]]
then
	echo "usage: make_appimage.sh path_to_linuxdeploy"
	exit 1
fi;

if [ -d AppDir ];
then
	rm -rf AppDir/
fi

rm *.AppImage
rm *.out
rm *.a
rm -rf objects/*;

if [ -f AppDir ];
then
	rm -rf AppDir
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

#export SIGN=1
#export SIGN_KEY=0xLAST8CHARSOFGPGKEYID

$1 --appdir AppDir
cp -r data AppDir/usr/share
mkdir -p AppDir/usr/share/metainfo
cp linuxdeploy/*.xml AppDir/usr/share/metainfo/
$1 --executable ./familiar_temple.out --appdir AppDir -i ./linuxdeploy/familiar_temple.png -d ./linuxdeploy/familiar_temple.desktop --output appimage
exit 0
