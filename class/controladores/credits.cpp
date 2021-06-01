#include "credits.h"
#include "../env.h"

const std::string credits::credits_text="A \"game\" by The Marlboro Man.\n\n"
"Music: \"temple of the mystics\" by jdagenet.\n\n"
"Tileset: \"hieroglyph sprites\" by Andor Salga.\n\n"
"Sound effects by Diode111 and Kayden Riggs.\n\n"
"Made in 48 hours for the 4th Familiar Game Jam.\n\n";


credits::credits(Sistema_estados& s, DLibV::Pantalla& pantalla)
	:Controlador_interface(s),
	txt_credits(pantalla.acc_renderer(), DLibV::Gestor_superficies::obtener(Recursos_graficos::RS_FUENTE_BASE), credits_text)
{
	txt_credits.establecer_posicion(32, 160);
}

bool credits::loop(const Input& input, float delta)
{
	if(input.es_senal_salida()) {

		return false;
	}

	if(input.es_input_down(Input::I_ESCAPE)) {

		solicitar_cambio_estado(Sistema_estados::estados::E_TITLE);
	}

	return true;
}

void credits::dibujar(DLibV::Pantalla& pantalla)
{
	pantalla.limpiar(0, 0, 0, 255);

	DLibV::Representacion_bitmap_estatica fondo;
	fondo.establecer_textura(DLibV::Gestor_texturas::obtener(Recursos_graficos::RT_TITLEPIC));
	fondo.establecer_posicion(0, 0, 800, 600);
	fondo.establecer_recorte(0, 0, 800, 600);
	fondo.volcar(pantalla);

	txt_credits.volcar(pantalla);
}
