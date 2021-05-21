#include "input.h"

void Input::configurar()
{
	//El input I_ESCAPE es la tecla SDLK_ESCAPE. Simple.
	mapa.insert(std::make_pair(I_ESCAPE, input_type{0, 0, SDL_SCANCODE_ESCAPE}));

	//keyboard controls...
	mapa.insert(std::make_pair(I_ESPACIO, input_type{0, 0, SDL_SCANCODE_SPACE}));
	mapa.insert(std::make_pair(I_IZQUIERDA, input_type{0, 0, SDL_SCANCODE_LEFT}));
	mapa.insert(std::make_pair(I_DERECHA, input_type{0, 0, SDL_SCANCODE_RIGHT}));
	mapa.insert(std::make_pair(I_ARRIBA, input_type{0, 0, SDL_SCANCODE_UP}));
	mapa.insert(std::make_pair(I_ABAJO, input_type{0, 0, SDL_SCANCODE_DOWN}));
	mapa.insert(std::make_pair(I_SALTAR, input_type{0, 0, SDL_SCANCODE_SPACE}));
	mapa.insert(std::make_pair(I_DISPARAR, input_type{0, 0, SDL_SCANCODE_LCTRL}));
	mapa.insert(std::make_pair(I_APUNTAR, input_type{0, 0, SDL_SCANCODE_LSHIFT}));
	mapa.insert(std::make_pair(I_RECARGAR, input_type{0, 0, SDL_SCANCODE_Z}));
	mapa.insert(std::make_pair(I_FULLSCREEN, input_type{0, 0, SDL_SCANCODE_F}));

	mapa.insert(std::make_pair(I_IZQUIERDA, input_type{1, 0, config.get_gamepad_left()}));
	mapa.insert(std::make_pair(I_DERECHA, input_type{1, 0, config.get_gamepad_right()}));
	mapa.insert(std::make_pair(I_ARRIBA, input_type{1, 0, config.get_gamepad_up()}));
	mapa.insert(std::make_pair(I_ABAJO, input_type{1, 0, config.get_gamepad_down()}));
	mapa.insert(std::make_pair(I_SALTAR, input_type{1, 0, config.get_gamepad_jump()}));
	mapa.insert(std::make_pair(I_DISPARAR, input_type{1, 0, config.get_gamepad_fire()}));
	mapa.insert(std::make_pair(I_APUNTAR, input_type{1, 0, config.get_gamepad_strafe()}));
	mapa.insert(std::make_pair(I_RECARGAR, input_type{1, 0, config.get_gamepad_reload()}));
}
