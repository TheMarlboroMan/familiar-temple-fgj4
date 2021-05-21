# familiar-temple-fgj4
Code and resources for this "oldie".

## Changelog

v1.0.0: March 2015, game gets done.
v1.1.0: May 2021, building as AppImage
v1.1.1: May 2021, gamepad support, tweaked scores, adds level number to level name

## building

You will need a c++11 compliant compiler, the SDL2 development libraries, the SDL2_Image development libraries and the the SDL2_Mixer development libraries. You can find everything you need in your package manager :).

./build.sh

## Compiling the project in windows:

Step 1: Preparing the makefile and libraries.

You will need the mingw32 compiler suite. You can get them from their site. Because this game is written in C++ you will need to install the g++ extensions.
You will need the following development libraries: SDL2, SDL2_Mixer, SDL2_Image. Download the ming32 versions of them.
Create a includes and libraries tree for the compiler. Mine's for example, go like this:
c:
	mingw_dev
		bin	-> Everything from the SDL2, SDL2_Mixer and SDL2_Image "bin" directories goes here.
		include
			SDL2 -> Everything from the SDL2, SDL2_Mixer and SDL2_Image "include/SDL2" directories go here.
		lib	-> Everything from the SDL2, SDL2_Mixer and SDL2_Image "lib" directories goes here.
That should do it.

Step 2: Compiling the source.

Unzip the source (let's assume you've done so in C:\familiar_temple.
Open a command line terminal. Cd into the sources: "cd c:\familiar_temple"
First we will be compiling the wrapper library used in the game: cd into libdan2 like "cd libdan2".
Edit the "makefile_win" file inside "libdan2" (beware, there's another in the project root that we will get to later).
Do the following changes:
	UNIDAD_DISCO=###The hard drive unit where mingw32 is installed goes here##
	INCLUDES=-I$(UNIDAD_DISCO):#Path to the mingw include libraries directory you created before goes here. In our example it would be "mingw_dev/include", without the hard drive#.
Save the file.
Make the wrapper library with "ming32-make.exe -f makefile_win". Lots of lines will appear o the screen. No error should appear.
When done, a "libDanSDL2.a" file will exist. Move it to the previous directory with "move libDanSDL2.a ../"
Go into the project root with "cd .."
Now, edit the other "makefile_win" file, this one is inside the project root.
Make the following changes:
	UNIDAD_DISCO=###The hard drive unit where mingw32 is installed goes here##
	MINGW32_INCLUDE=-I$(UNIDAD_DISCO):#Path to the mingw include libraries directory you created before goes here. In our example it would be "mingw_dev/include", without the hard drive#.
	MINGW32_LIB=-I$(UNIDAD_DISCO):#Path to the mingw lib directory you created before goes here. In our example it would be "mingw_dev/lib", without the hard drive#.
Save the file.
Make the game with "ming32-make.exe -f makefile_win". Lots of lines will appear o the screen. No error should appear.
Okay, you're done!.

Step 3: Grabbing the dlls...

You will these libraries to play the game:

	libFLAC-8.dll
	libjpeg-9.dll
	libmikmod-2.dll
	libmodplug-1.dll
	libogg-0.dll
	libpng16-16.dll
	libtiff-5.dll
	libvorbis-0.dll
	libvorbisfile-3.dll
	libwebp-4.dll
	SDL2.dll
	SDL2_image.dll
	SDL2_mixer.dll
	smpeg2.dll
	zlib1.dll

All of them you can find with the SDL2, SDL2_Mixer and SDL2_Image binaries in their sites. Just put them where the executable is.

You´re done :).

If you have any problems, please, contact me on gamejolt under "The Marlboro Man" user.
