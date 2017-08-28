#ifndef CONTROLADOR_FIN_2
#define CONTROLADOR_FIN_2

#include "controlador_interface.h"
#include "../recursos/recursos.h"

class Controlador_fin_2:public Controlador_interface
{
	private:

	public:
	
	Controlador_fin_2(Sistema_estados& s): Controlador_interface(s) {}
	virtual void preloop(const Input& input, float delta) {}
	virtual void postloop() {}

	virtual bool loop(const Input& input, float delta);
	virtual void dibujar(DLibV::Pantalla& pantalla);
};

#endif
