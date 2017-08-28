#include "controlador_fin_2.h"

bool Controlador_fin_2::loop(const Input& input, float delta)
{
	if(input.es_input_down(Input::I_DISPARAR) || input.es_input_down(Input::I_SALTAR) || input.es_senal_salida() || input.es_input_down(Input::I_ESCAPE))
	{
		solicitar_cambio_estado(Sistema_estados::estados::E_TITLE);
	}
	return true;
}

void Controlador_fin_2::dibujar(DLibV::Pantalla& pantalla)
{
	pantalla.limpiar(0, 0, 0, 255);
	
	DLibV::Representacion_bitmap_estatica fondo;
	fondo.establecer_textura(DLibV::Gestor_texturas::obtener(Recursos_graficos::RT_END2PIC));
	fondo.establecer_posicion(0, 0, 800, 600);
	fondo.establecer_recorte(0, 0, 800, 600);
	fondo.volcar(pantalla);

	std::string cadena_game_over="You have won this time but...\n\n\n\nAs foretold, the priest will\n\nreturn to the Earth again.\n\n\n\nOnly by collecting all the Ankhs\n\nwill you banish him forever!.";

	DLibV::Representacion_texto_auto_estatica txt_intro(pantalla.acc_renderer(), DLibV::Gestor_superficies::obtener(Recursos_graficos::RS_FUENTE_BASE), cadena_game_over);
	txt_intro.establecer_posicion(16, 16);
	txt_intro.volcar(pantalla);
}
