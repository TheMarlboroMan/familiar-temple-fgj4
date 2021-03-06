#ifndef PROYECTO_SDL2_DERIVADA_CARGADOR_RECURSOS_H
#define PROYECTO_SDL2_DERIVADA_CARGADOR_RECURSOS_H

#include "../base_proyecto/cargador_recursos_base.h"
#include "../env.h"

class Cargador_recursos:public Cargador_recursos_base
{
	protected:

	std::string obtener_ruta_audio() const {return env::make_data_path("data/recursos/audio.txt");}
	std::string obtener_ruta_musica() const {return env::make_data_path("data/recursos/musica.txt");}
	std::string obtener_ruta_texturas() const {return env::make_data_path("data/recursos/texturas.txt");}
	std::string obtener_ruta_superficies() const {return env::make_data_path("data/recursos/superficies.txt");}

	public:

	Cargador_recursos(DLibV::Pantalla &p):Cargador_recursos_base(p) {}
};

#endif
