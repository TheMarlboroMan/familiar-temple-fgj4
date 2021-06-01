#include "logos.h"
#include "../env.h"


logos::logos(Sistema_estados& s, DLibV::Pantalla& _screen)
	:Controlador_interface(s),
	made_for{
		_screen.acc_renderer(), 
		DLibV::Gestor_superficies::obtener(Recursos_graficos::RS_FONT_LARGE), 
		"Made for the Familiar Game Jam 4"
	},
	txt_author{
		_screen.acc_renderer(), 
		DLibV::Gestor_superficies::obtener(Recursos_graficos::RS_FONT_LARGE), 
		"A game by The Marlboro Man"
	}
{
	auto screen_w=_screen.acc_simulacro_caja().w;
	int made_for_x=(screen_w / 2) - (made_for.acc_ancho_area() / 2);
	int event_logo_x=(screen_w / 2) - (479 / 2);
	int author_x=(screen_w / 2) - (txt_author.acc_ancho_area() / 2);


	made_for.establecer_posicion(made_for_x, 64);
	made_for.establecer_alpha(0);
	made_for.establecer_modo_blend(DLibV::Representacion::BLEND_ALPHA);

	event_logo.establecer_textura(DLibV::Gestor_texturas::obtener(Recursos_graficos::RT_EVENT_LOGO));
	event_logo.establecer_posicion(event_logo_x, 120, 479, 304);
	event_logo.establecer_recorte(0, 0, 479, 304);
	event_logo.establecer_alpha(0);
	event_logo.establecer_modo_blend(DLibV::Representacion::BLEND_ALPHA);
	
	txt_author.establecer_posicion(author_x, 460);
	txt_author.establecer_alpha(0);
	txt_author.establecer_modo_blend(DLibV::Representacion::BLEND_ALPHA);

}

bool logos::loop(const Input& input, float delta)
{
	if(input.es_senal_salida()) {
	
		return false;
	}

	if(input.es_input_down(Input::I_ESCAPE) || 
		input.es_input_down(Input::I_SALTAR) || 
		input.es_input_down(Input::I_DISPARAR)
	) {

		solicitar_cambio_estado(Sistema_estados::estados::E_TITLE);
	}
	else {
		run_logic(delta);
	}
	
	return true;
}

void logos::dibujar(
	DLibV::Pantalla& _screen
) {
	_screen.limpiar(0, 0, 0, 255);

	made_for.volcar(_screen);
	event_logo.volcar(_screen);
	txt_author.volcar(_screen);
}

void logos::run_logic(float _delta) {

	switch(step) {
	
		case steps::start:
		
			time+=_delta;
		
			if(time > 0.8f) {
			
				step=steps::show_event;
				ltf.reset(0.f, 255.f, 110.f);
			}
		
		break;
		case steps::show_event:
		
			ltf.tic(_delta);
			
			made_for.establecer_alpha(ceil(ltf()));
			event_logo.establecer_alpha(ceil(ltf()));
			
			if(ltf.is_done()) {
				step=steps::wait;
				time=0.f;
			}
		
		break;
		case steps::wait:
		
			time+=_delta;
			if(time > 0.8f) {
			
				step=steps::show_author;
				ltf.reset(0.f, 255.f, 110.f);
			}
		
		break;
		case steps::show_author:
		
			ltf.tic(_delta);
			
			txt_author.establecer_alpha(ceil(ltf()));
			
			if(ltf.is_done()) {
				step=steps::wait_again;
				time=0.f;
			}
		break;
		case steps::wait_again:
			
			time+=_delta;
			if(time > 0.8f) {
			
				step=steps::fadeout;
				ltf.reset(255.f, 0.f, -110.f);
			}
		
		break;
		case steps::fadeout:
		
			ltf.tic(_delta);
			
			made_for.establecer_alpha(floor(ltf()));
			event_logo.establecer_alpha(floor(ltf()));
			txt_author.establecer_alpha(floor(ltf()));
			
			if(ltf.is_done()) {
				step=steps::end;
				time=0.f;
			}
		break;
		case steps::end:
		
			time+=_delta;
			if(time > 1.4f) {
			
				solicitar_cambio_estado(Sistema_estados::estados::E_TITLE);
			}
		break;
	}
}
