#ifndef PROYECTO_SDL2_BASE_INPUT_H
#define PROYECTO_SDL2_BASE_INPUT_H

#include <map>
#include "../../libdan2/input/controles_sdl/controles_sdl.h"

/*Abstrae los sistemas de input usando la clase Controles_SDL. Cada input del
enum de turno se puede asignar a uno o más valores SDLK_xxx, según lo mismo
pueda hacerse con varias teclas, joystick o no. Esta clase es la propietaria
de la instancia de controles_SDL.
*/

class Input_base
{
	////////////////////////
	//Definiciones....

	public:

	struct input_type {
		int type, //0 => key, 1 => joystick.
			device,
			code;
	};

	typedef std::multimap<unsigned int, input_type> tipo_mapa;
	typedef std::multimap<unsigned int, input_type>::const_iterator tipo_iterador;

	private:

	typedef std::pair <tipo_iterador, tipo_iterador> tipo_par;

	////////////////////////
	//Propiedades. --

	private:
	DLibI::Controles_SDL controles_sdl;

	protected:
	tipo_mapa mapa;

	////////////////////////
	//Métodos

	private:

	tipo_par obtener(unsigned int) const;

	public:

	//Este es el método que tendremos que extender.
	virtual void configurar()=0;

	/* Todas estas vamos a imaginar que son finales, ok?... */

	void turno();

	bool es_senal_salida() const;
	bool es_input_down(unsigned int) const;
	bool es_input_up(unsigned int) const;
	bool es_input_pulsado(unsigned int) const;

	bool es_tecla_down(unsigned int i) const {return controles_sdl.es_tecla_down(i);}
	bool es_tecla_up(unsigned int i) const {return controles_sdl.es_tecla_up(i);}
	bool es_tecla_pulsada(unsigned int i) const {return controles_sdl.es_tecla_pulsada(i);}

	bool hay_eventos_teclado_down() const {return controles_sdl.recibe_eventos_teclado_down();}

	DLibI::Controles_SDL& acc_controles_sdl() {return controles_sdl;}

	Input_base() {}
};

#endif
