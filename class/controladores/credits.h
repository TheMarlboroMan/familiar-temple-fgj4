#pragma once

#include "controlador_interface.h"
#include "../recursos/recursos.h"

class credits:public Controlador_interface
{
	private:

	static const std::string credits_text;

	DLibV::Representacion_texto_auto_estatica txt_credits;

	public:

	credits(Sistema_estados& s, DLibV::Pantalla&p);

	virtual void preloop(const Input& input, float delta) {}
	virtual void postloop() {}

	virtual bool loop(const Input& input, float delta);
	virtual void dibujar(DLibV::Pantalla& pantalla);
};

