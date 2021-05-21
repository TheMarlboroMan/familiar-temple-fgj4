#include "controlador_title.h"
#include "../env.h"

const std::string Controlador_title::cadena_intro="F A M I L I A R    T E M P L E\n\n\n\n"
"The evil priest Nebneteru has returned to Earth once more.\n\nVenture inside the evil temple and defeat him before he regains his power!.";
const std::string Controlador_title::cadena_controles="Use ARROW KEYS to move.\n\n"
"SPACE / BUTTON 3 to jump.\n\n"
"LEFT CONTROL / RIGHT TRIGGER to fire.\n\n"
"LEFT SHIFT / RIGHT BUMPER to strafe.\n\n"
"Z / BUTTON 4 / Double tap strafe to reload.\n\n"
"F to cycle between the 3 screen modes (you may not notice the difference).\n\n"
"Find the exit in each level and vanquish the priest in the end.\n\nCollect bonus for extra points.\n\nGet an extra life each 1500 points.\n\nJump over deadly floors and projectiles.\n\n\n\nPress JUMP or FIRE to play.";
const std::string Controlador_title::cadena_creditos="A \"game\" by The Marlboro Man.\n\nMusic: \"temple of the mystics\" by jdagenet.\n\nTileset: \"hieroglyph sprites\" by Andor Salga.\n\nSound effects by Diode111 and Kayden Riggs.\n\nMade for the 4th Familiar Game Jam.\n\n\n";


Controlador_title::Controlador_title(Sistema_estados& s, DLibV::Pantalla& pantalla)
	:Controlador_interface(s),
	txt_intro(pantalla.acc_renderer(), DLibV::Gestor_superficies::obtener(Recursos_graficos::RS_FUENTE_BASE), cadena_intro),
	txt_controles(pantalla.acc_renderer(), DLibV::Gestor_superficies::obtener(Recursos_graficos::RS_FUENTE_BASE), cadena_controles),
	txt_creditos(pantalla.acc_renderer(), DLibV::Gestor_superficies::obtener(Recursos_graficos::RS_FUENTE_BASE),
		cadena_creditos+std::string{"2015, v"}+std::to_string(env::major_version)+"."
		+std::to_string(env::minor_version)+"."
		+std::to_string(env::patch_version))
{
	txt_intro.establecer_posicion(32, 32);
	txt_controles.establecer_posicion(32, 160);
	txt_creditos.establecer_posicion(420, 480);
}

bool Controlador_title::loop(const Input& input, float delta)
{
	if(input.es_senal_salida() || input.es_input_down(Input::I_ESCAPE))
	{
		return false;
	}
	else
	{
		if(input.es_input_down(Input::I_DISPARAR) || input.es_input_down(Input::I_SALTAR))
		{
			solicitar_cambio_estado(Sistema_estados::estados::E_JUEGO);
		}
		return true;
	}
}

void Controlador_title::dibujar(DLibV::Pantalla& pantalla)
{
	pantalla.limpiar(0, 0, 0, 255);

	DLibV::Representacion_bitmap_estatica fondo;
	fondo.establecer_textura(DLibV::Gestor_texturas::obtener(Recursos_graficos::RT_TITLEPIC));
	fondo.establecer_posicion(0, 0, 800, 600);
	fondo.establecer_recorte(0, 0, 800, 600);
	fondo.volcar(pantalla);

	txt_intro.volcar(pantalla);
	txt_controles.volcar(pantalla);
	txt_creditos.volcar(pantalla);
}
