#include "configuracion.h"

const std::string Configuracion::CLAVE_IDIOMA="idioma";

Configuracion::Configuracion(): Configuracion_base()
{

}

Configuracion::~Configuracion()
{

}

void Configuracion::grabar_valores_configuracion(std::ofstream& archivo, const std::string& SEPARADOR)
{
	archivo<<CLAVE_IDIOMA<<SEPARADOR<<idioma<<std::endl;
}

void Configuracion::procesar_clave_y_valor(const std::string& clave, const std::string& valor)
{
	if(clave.compare(CLAVE_IDIOMA)==0)
	{
		this->idioma=std::atoi(valor.c_str());
		return;
	}

	if(clave.compare("gamepad_up")==0) {

		gamepad_up=std::atoi(valor.c_str());
		return;
	}

	if(clave.compare("gamepad_right")==0) {

		gamepad_right=std::atoi(valor.c_str());
		return;
	}

	if(clave.compare("gamepad_down")==0) {

		gamepad_down=std::atoi(valor.c_str());
		return;
	}

	if(clave.compare("gamepad_left")==0) {

		gamepad_left=std::atoi(valor.c_str());
		return;
	}

	if(clave.compare("gamepad_jump")==0) {

		gamepad_jump=std::atoi(valor.c_str());
		return;
	}

	if(clave.compare("gamepad_fire")==0) {

		gamepad_fire=std::atoi(valor.c_str());
		return;
	}

	if(clave.compare("gamepad_strafe")==0) {

		gamepad_strafe=std::atoi(valor.c_str());
		return;
	}

	if(clave.compare("gamepad_reload")==0) {

		gamepad_reload=std::atoi(valor.c_str());
		return;
	}

	if(clave.compare("gamepad_focus")==0) {

		gamepad_focus=std::atoi(valor.c_str());
		return;
	}

}

void Configuracion::asignar_valores_por_defecto()
{
	idioma=0;
}
