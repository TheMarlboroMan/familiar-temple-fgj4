#ifndef REPRESENTADOR_H
#define REPRESENTADOR_H

#include "representable.h"
#include "../herramientas_proyecto/tabla_sprites.h"
#include "../sistemas/powers.h"

#include <string>

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
		
	int left_x{0},
		left_y{0},
		left_w{0},
		left_h{0},
		center_x{0},
		center_y{0},
		center_w{0},
		center_h{0},
		ammo_x{0},
		ammo_y{0},
		ammo_reserve_x{0},
		ammo_reserve_y{0},
		focus_bar_x{0},
		focus_bar_y{0},
		focus_bar_w{0},
		focus_bar_h{0},
		focus_power_x{0},
		focus_power_y{0},
		health_x{0},
		health_y{0},
		health_margin{0},
		timer_x{0},
		timer_y{0},
		ankh_icon_x{0},
		ankh_icon_y{0},
		ankh_x{0},
		ankh_y{0},
		score_x{0},
		score_y{0},
		level_name_box_x{0},
		level_name_box_y{0},
		level_name_box_w{0},
		level_name_box_h{0},
		level_name_x{0},
		level_name_y{0},
		lives_icon_x{0},
		lives_icon_y{0},
		lives_x{0},
		lives_y{0};
		
		
	
		// Interface pública

	public:

	void reload_data();

	void hud_overlay(DLibV::Pantalla&, bool);
	void hud_ammo(DLibV::Pantalla&, const std::vector<Representable *>&, int);
	void hud_focus(DLibV::Pantalla&, int, const Frame_sprites&, bool);
	void hud_health(DLibV::Pantalla&, int, const Frame_sprites&, const Frame_sprites&);
	void hud_timer(DLibV::Pantalla&, float, bool);
	void hud_ankh(DLibV::Pantalla& pantalla, unsigned int actuales, unsigned int en_nivel, const Frame_sprites& fs);
	void hud_score(DLibV::Pantalla& pantalla, unsigned int);
	void hud_level_name(DLibV::Pantalla& pantalla, const std::string&, int, unsigned int, const Frame_sprites& fs);
	void power_overlay(DLibV::Pantalla&, bool, powers::power_type, const Frame_sprites&, const Frame_sprites&);

	unsigned int generar_vista(DLibV::Pantalla&, DLibV::Camara&, const std::vector<Representable *>&);

	Representador();
	~Representador();
};

#endif
