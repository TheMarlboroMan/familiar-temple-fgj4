#include "controlador_game_over.h"

bool Controlador_game_over::loop(const Input& input, float delta)
{
	if(input.es_input_down(Input::I_DISPARAR) || input.es_input_down(Input::I_SALTAR)) 
	{
		solicitar_cambio_estado(Sistema_estados::estados::E_TITLE);
	}
	return true;
}

void Controlador_game_over::dibujar(DLibV::Pantalla& pantalla)
{
	pantalla.limpiar(0, 0, 0, 255);

	DLibV::Representacion_bitmap_estatica fondo;
	fondo.establecer_textura(DLibV::Gestor_texturas::obtener(Recursos_graficos::RT_GAMEOVERPIC));
	fondo.establecer_posicion(0, 0, 800, 600);
	fondo.establecer_recorte(0, 0, 800, 600);
	fondo.volcar(pantalla);

	std::string cadena_game_over="You failed.\n\nAs the Solar Ship rounds the world to come back at the same place, you may try again.";

	DLibV::Representacion_texto_auto_estatica txt_intro(pantalla.acc_renderer(), DLibV::Gestor_superficies::obtener(Recursos_graficos::RS_FUENTE_BASE), cadena_game_over);
	txt_intro.establecer_posicion(32, 530);
	txt_intro.volcar(pantalla);
}
