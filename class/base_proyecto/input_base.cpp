#include "input_base.h"

void Input_base::turno()
{
	controles_sdl.recoger();
}

Input_base::tipo_par Input_base::obtener(unsigned int i) const
{
	tipo_par resultado=mapa.equal_range(i);
	return resultado;
}

bool Input_base::es_senal_salida() const
{
	return controles_sdl.es_senal_salida();
}

bool Input_base::es_input_down(unsigned int i) const
{
	tipo_par resultado=obtener(i);
	for(tipo_mapa::const_iterator it=resultado.first; it!=resultado.second; ++it)
	{
		if(controles_sdl.es_tecla_down(it->second)) return true;
	}

	return false;
}

bool Input_base::es_input_up(unsigned int i) const
{
	tipo_par resultado=obtener(i);
	for(tipo_mapa::const_iterator it=resultado.first; it!=resultado.second; ++it)
		if(controles_sdl.es_tecla_up(it->second)) return true;

	return false;
}

bool Input_base::es_input_pulsado(unsigned int i) const
{
	tipo_par resultado=obtener(i);
	for(tipo_mapa::const_iterator it=resultado.first; it!=resultado.second; ++it)
		if(controles_sdl.es_tecla_pulsada(it->second)) return true;

	return false;
}
