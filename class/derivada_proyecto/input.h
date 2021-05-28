#ifndef PROYECTO_SDL2_DERIVADA_INPUT_H
#define PROYECTO_SDL2_DERIVADA_INPUT_H

/*La clase que controla el input. La interfaz pública la sacamos del Input_base
y es esta:

bool es_senal_salida() const;
bool es_input_down(unsigned int) const;
bool es_input_up(unsigned int) const;
bool es_input_pulsado(unsigned int) const;

bool es_tecla_down(unsigned int i) const;
bool es_tecla_up(unsigned int i) const;
bool es_tecla_pulsada(unsigned int i);

bool hay_eventos_teclado_down();
*/

#include "../base_proyecto/input_base.h"
#include "configuracion.h"

class Input:public Input_base
{
	/////////////////
	//Definiciones...

	public:

	enum inputs{
I_ESCAPE=0,
I_ESPACIO,
I_IZQUIERDA,
I_DERECHA,
I_ARRIBA,
I_ABAJO,
I_SALTAR,
I_DISPARAR,
I_APUNTAR,
I_RECARGAR,
I_FOCUS,
I_FULLSCREEN
};

	const Configuracion& config;

	///////////////////
	// Métodos

	public:

	virtual void configurar();
	Input(const Configuracion& _config):Input_base(), config{_config} {}
};

#endif
