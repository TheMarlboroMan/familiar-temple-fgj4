#ifndef INPUT_USUARIO_H
#define INPUT_USUARIO_H

struct Input_usuario
{
	short int mov_horizontal;
	short int mov_vertical;
	short int keyup_horizontal;
	bool saltar;
	bool disparar;
	bool apuntar;
	bool keydown_apuntar;

	Input_usuario():mov_horizontal(0), mov_vertical(0), keyup_horizontal(0), saltar(false), disparar(false), apuntar(false), keydown_apuntar(false)
	{}
};

#endif
