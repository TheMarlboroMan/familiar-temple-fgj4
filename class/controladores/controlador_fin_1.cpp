#include "controlador_fin_1.h"

bool Controlador_fin_1::loop(const Input& input, float delta)
{
	if(input.es_input_down(Input::I_DISPARAR) || input.es_input_down(Input::I_SALTAR) || input.es_senal_salida() || input.es_input_down(Input::I_ESCAPE))
	{
		solicitar_cambio_estado(Sistema_estados::estados::E_TITLE);
	}
	return true;
}

void Controlador_fin_1::dibujar(DLibV::Pantalla& pantalla)
{
	pantalla.limpiar(0, 0, 0, 255);
	
	DLibV::Representacion_bitmap_estatica fondo;
	fondo.establecer_textura(DLibV::Gestor_texturas::obtener(Recursos_graficos::RT_END1PIC));
	fondo.establecer_posicion(0, 0, 800, 600);
	fondo.establecer_recorte(0, 0, 800, 600);
	fondo.volcar(pantalla);


	std::string cadena_game_over="Congratulations!!\n\nYou vanquished the priest\n\nand won the game!.\n\n\n\nAll this gold is yours\n\nfor no reason!!.";

	DLibV::Representacion_texto_auto_estatica txt_intro(pantalla.acc_renderer(), DLibV::Gestor_superficies::obtener(Recursos_graficos::RS_FUENTE_BASE), cadena_game_over);
	txt_intro.establecer_posicion(16, 16);
	txt_intro.volcar(pantalla);
}
