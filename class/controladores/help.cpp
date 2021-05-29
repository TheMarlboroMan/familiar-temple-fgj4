#include "help.h"
#include "../env.h"

const std::string help::help_text="Use ARROW KEYS to move.\n\n"
"SPACE / BUTTON 3 to jump.\n\n"
"LEFT CONTROL / RIGHT TRIGGER to fire.\n\n"
"LEFT SHIFT / RIGHT BUMPER to strafe.\n\n"
"Z / BUTTON 3 / Double tap strafe to reload.\n\n"
"X / BUTTON 4 / activate focus power.\n\n"
"A / BUTTON 6 / last picked up weapon.\n\n"
"F to cycle between the 3 screen modes (you may not notice the difference).\n\n\n\n"
"Collect bonus for extra points.\n\n"
"Land shots, collect bonus and get hit to fill the focus bar.\n\n"
"Activate focus powers with full focus bar.\n\n"
"Get an extra life 1500, 4500, 9000, 15000 and more points.\n\n"
"Jump over deadly floors and projectiles.\n\n"
"Find the exit in each level and vanquish the priest in the end.";

help::help(Sistema_estados& s, DLibV::Pantalla& pantalla)
	:Controlador_interface(s),
	txt_help(pantalla.acc_renderer(), DLibV::Gestor_superficies::obtener(Recursos_graficos::RS_FUENTE_BASE), help_text)
{
	txt_help.establecer_posicion(32, 160);
}

bool help::loop(const Input& input, float delta)
{
	if(input.es_senal_salida()) {

		return false;
	}

	if(input.es_input_down(Input::I_ESCAPE)) {

		solicitar_cambio_estado(Sistema_estados::estados::E_TITLE);
	}

	return true;
}

void help::dibujar(DLibV::Pantalla& pantalla)
{
	pantalla.limpiar(0, 0, 0, 255);

	DLibV::Representacion_bitmap_estatica fondo;
	fondo.establecer_textura(DLibV::Gestor_texturas::obtener(Recursos_graficos::RT_TITLEPIC));
	fondo.establecer_posicion(0, 0, 800, 600);
	fondo.establecer_recorte(0, 0, 800, 600);
	fondo.volcar(pantalla);

	txt_help.volcar(pantalla);
}
