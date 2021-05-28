#ifndef REPRESENTADOR_H
#define REPRESENTADOR_H

#include "representable.h"
#include "../herramientas_proyecto/tabla_sprites.h"
#include "../sistemas/powers.h"

/*El representador funciona de la siguiente forma: desde el controlador 
generamos un array de punteros a "Representables". Se le pasa el array al 
representador que primero lo ordena por profundidad y luego genera un bloque de
transformación que pasa y procesa para cada uno de ellos.

El representador, por supuesto, sólo funciona con aquellas entidades que sean
"Representable", esto es, para todo aquello que forme parte del mundo. HUDS,
mensajes y otras cosas irían representadas de otro modo.
*/

class Representador
{
	/////////////////////////
	// Propiedades
	
	private:

	Bloque_transformacion_representable bloque_transformacion;

	/////////////////////////
	// Internos...

	private:

	/////////////////////////
	// Interface pública

	public:

	void generar_hud_municion(DLibV::Pantalla&, const std::vector<Representable *>&);
	void generar_hud_nivel(DLibV::Pantalla&, const std::string& nombre, unsigned int segundos, unsigned int decimas, unsigned int puntuacion);
	void generar_hud_vida(DLibV::Pantalla& pantalla, unsigned int energia, unsigned int vidas, const Frame_sprites& fs_energia, const Frame_sprites& fs_vidas);
	void generar_hud_ankh(DLibV::Pantalla& pantalla, unsigned int actuales, unsigned int en_nivel, unsigned int total, const Frame_sprites& fs);
	void focus_hud(DLibV::Pantalla& pantalla, unsigned int current, powers::power_type);

	unsigned int generar_vista(DLibV::Pantalla&, DLibV::Camara&, const std::vector<Representable *>&);

	Representador();
	~Representador();
};

#endif
