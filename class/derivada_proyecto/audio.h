#ifndef PROYECTO_SDL2_DERIVADA_AUDIO_H
#define PROYECTO_SDL2_DERIVADA_AUDIO_H

#include "../base_proyecto/audio_base.h"

class Audio:public Audio_base
{
	private:

	Audio();

	public:

	//Sonidos que son usados por el código en si, en contraste con los
	//sonidos que puedan cargarse dinámicamente...
	enum sonidos {
		S_MIN=0,
		//Sonidos de código aquí...
		S_MAX};
};

#endif
