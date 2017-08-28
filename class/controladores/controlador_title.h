#ifndef CONTROLADOR_TITLE
#define CONTROLADOR_TITLE

#include "controlador_interface.h"
#include "../recursos/recursos.h"

class Controlador_title:public Controlador_interface
{
	private:

	static const std::string cadena_intro;
	static const std::string cadena_controles;
	static const std::string cadena_creditos;

	DLibV::Representacion_texto_auto_estatica txt_intro;
	DLibV::Representacion_texto_auto_estatica txt_controles;
	DLibV::Representacion_texto_auto_estatica txt_creditos;

	public:
	
	Controlador_title(Sistema_estados& s, DLibV::Pantalla&p);

	virtual void preloop(const Input& input, float delta) {}
	virtual void postloop() {}

	virtual bool loop(const Input& input, float delta);
	virtual void dibujar(DLibV::Pantalla& pantalla);
};

#endif
