#ifndef INPUT_USUARIO_H
#define INPUT_USUARIO_H

struct Input_usuario
{
	short int mov_horizontal{0},
		mov_vertical{0},
		keyup_horizontal{0};
	bool saltar{false},
		disparar{false},
		apuntar{false},
		keydown_apuntar{false},
		recargar{false};
};

#endif
