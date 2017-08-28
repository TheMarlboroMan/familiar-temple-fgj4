#include "input.h"

void Input::configurar()
{
	//El input I_ESCAPE es la tecla SDLK_ESCAPE. Simple.
	mapa.insert(std::make_pair(I_ESCAPE, SDL_SCANCODE_ESCAPE));

	mapa.insert(std::make_pair(I_ESPACIO, SDL_SCANCODE_SPACE));

	mapa.insert(std::make_pair(I_IZQUIERDA, SDL_SCANCODE_LEFT));
	mapa.insert(std::make_pair(I_DERECHA, SDL_SCANCODE_RIGHT));
	mapa.insert(std::make_pair(I_ARRIBA, SDL_SCANCODE_UP));
	mapa.insert(std::make_pair(I_ABAJO, SDL_SCANCODE_DOWN));
	mapa.insert(std::make_pair(I_SALTAR, SDL_SCANCODE_SPACE));
	mapa.insert(std::make_pair(I_DISPARAR, SDL_SCANCODE_LCTRL));
	mapa.insert(std::make_pair(I_APUNTAR, SDL_SCANCODE_LSHIFT));
	mapa.insert(std::make_pair(I_RECARGAR, SDL_SCANCODE_Z));
	mapa.insert(std::make_pair(I_FULLSCREEN, SDL_SCANCODE_F));
}
