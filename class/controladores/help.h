#pragma once

#include "controlador_interface.h"
#include "../recursos/recursos.h"

class help:public Controlador_interface
{
	private:

	static const std::string help_text;

	DLibV::Representacion_texto_auto_estatica txt_help;

	public:

	help(Sistema_estados& s, DLibV::Pantalla&p);

	virtual void preloop(const Input& input, float delta) {}
	virtual void postloop() {}

	virtual bool loop(const Input& input, float delta);
	virtual void dibujar(DLibV::Pantalla& pantalla);
};

