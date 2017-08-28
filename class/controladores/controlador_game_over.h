#ifndef CONTROLADOR_GAME_OVER
#define CONTROLADOR_GAME_OVER

#include "controlador_interface.h"
#include "../recursos/recursos.h"

class Controlador_game_over:public Controlador_interface
{
	private:

	public:
	
	Controlador_game_over(Sistema_estados& s): Controlador_interface(s) {}
	virtual void preloop(const Input& input, float delta) {}
	virtual void postloop() {}

	virtual bool loop(const Input& input, float delta);
	virtual void dibujar(DLibV::Pantalla& pantalla);
};

#endif
