#ifndef CONTROLADOR_INTERFACE_H
#define CONTROLADOR_INTERFACE_H

#include "../derivada_proyecto/input.h"
#include "../../libdan2/libDan2.h"
#include "../derivada_proyecto/audio.h"
#include "../sistemas/sistema_estados.h"
#include <string>

class Controlador_interface
{
	private:

	std::string debug;
	Sistema_estados& SES;
		
	public:

	Controlador_interface(Sistema_estados& s): SES(s) {}
	virtual ~Controlador_interface() {}

	void solicitar_cambio_estado(Sistema_estados::estados e) {SES.cambiar_estado(e);}

	void mut_debug(const std::string& c) {debug=c;}
	const std::string& acc_debug() const {return debug;}

	virtual void preloop(const Input& input, float delta)=0;
	virtual bool loop(const Input& input, float delta)=0;
	virtual void postloop()=0;
	virtual void dibujar(DLibV::Pantalla& pantalla)=0;
};


#endif
