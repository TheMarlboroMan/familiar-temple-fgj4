#include "controlador_title.h"
#include "../env.h"

#include <sstream>
#include <iomanip>

const std::string Controlador_title::cadena_intro=
"         The evil priest Nebneteru has returned to Earth once more."
"\n\n"
"Venture inside the evil temple and defeat him before he regains his power!.";
const std::string Controlador_title::cadena_controles="Press F1 for help\n\n"
"Press F2 for credits\n\n"
"Press JUMP or FIRE to play.";

Controlador_title::Controlador_title(
	Sistema_estados& s, 
	hi_score_manager& _hi_score,
	DLibV::Pantalla& pantalla
)
	:Controlador_interface(s),
	hi_scores{_hi_score},
	txt_intro(pantalla.acc_renderer(), DLibV::Gestor_superficies::obtener(Recursos_graficos::RS_FUENTE_BASE), cadena_intro),
	txt_controles(pantalla.acc_renderer(), DLibV::Gestor_superficies::obtener(Recursos_graficos::RS_FUENTE_BASE), cadena_controles),
	txt_version(pantalla.acc_renderer(), DLibV::Gestor_superficies::obtener(Recursos_graficos::RS_FUENTE_BASE),
		std::string{"v"}+std::to_string(env::major_version)+"."
		+std::to_string(env::minor_version)+"."
		+std::to_string(env::patch_version)+" 2015"),
	txt_hi_score(pantalla.acc_renderer(), DLibV::Gestor_superficies::obtener(Recursos_graficos::RS_FUENTE_BASE), "")
{

	txt_intro.establecer_posicion(80, 250);
	txt_controles.establecer_posicion(32, 300);
	txt_hi_score.establecer_posicion(32, 380);
	txt_version.establecer_posicion(680, 580);
		
	game_logo_top.establecer_textura(DLibV::Gestor_texturas::obtener(Recursos_graficos::RT_GAME_LOGO));
	game_logo_top.establecer_posicion(logo_top_x, -300, 734, logo_top_h);
	game_logo_top.establecer_recorte(0, 0, 734, logo_top_h);
	game_logo_top.establecer_modo_blend(DLibV::Representacion::BLEND_ALPHA);
	
	game_logo_bottom.establecer_textura(DLibV::Gestor_texturas::obtener(Recursos_graficos::RT_GAME_LOGO));
	game_logo_bottom.establecer_posicion(logo_bottom_x, 900, 551, 133);
	game_logo_bottom.establecer_recorte(0, 120, 551, 133);
	game_logo_bottom.establecer_modo_blend(DLibV::Representacion::BLEND_ALPHA);
	
	ltf_logo_top.reset(-300.f, 20.f, 300.f);
	ltf_logo_bottom.reset(900.f, 100.f, -400.f);
	
	load_hi_scores();
}

bool Controlador_title::loop(const Input& input, float delta)
{
	if(input.es_senal_salida()) {
		return false;
	}
	
	switch(state) {
	
		case states::animate_logo:
			loop_animate_logo(input, delta);	
			return true;				
		break;
		case states::title:
			return loop_title(input, delta);	
		break;
	}
	
	return true;
}

void Controlador_title::loop_animate_logo(const Input& input, float delta) {

	if(input.es_input_down(Input::I_ESCAPE)) {
	
		Audio::insertar_sonido(
			DLibA::Estructura_sonido(
				DLibA::Gestor_recursos_audio::obtener_sonido(Recursos_audio::RS_DISPARO_ENEMIGO), 
				100
			)
		);
		
		game_logo_top.establecer_posicion(logo_top_x, 20, 734, logo_top_h);
		game_logo_bottom.establecer_posicion(logo_bottom_x, 100, 551, 133);
	
		state=states::title;
		return;
	}
	
	ltf_logo_top.tic(delta);
	ltf_logo_bottom.tic(delta);
	
	if(ltf_logo_top.is_done()) {
	
		if(!top_done) {
	
			Audio::insertar_sonido(
				DLibA::Estructura_sonido(
					DLibA::Gestor_recursos_audio::obtener_sonido(Recursos_audio::RS_DISPARO_ENEMIGO), 
					100
				)
			);
			
			top_done=true;
		}
	}
	else {
	
		game_logo_top.establecer_posicion(logo_top_x, ltf_logo_top(), 734, logo_top_h);
	}
	
	if(ltf_logo_bottom.is_done()) {
	
		if(!bottom_done) {
					
			Audio::insertar_sonido(
				DLibA::Estructura_sonido(
					DLibA::Gestor_recursos_audio::obtener_sonido(Recursos_audio::RS_DISPARO_ENEMIGO), 
					100
				)   
			);
			
			bottom_done=true;
		}
	}
	else {
	
		game_logo_bottom.establecer_posicion(logo_bottom_x, ltf_logo_bottom(), 551, 133);
	}
	
	if(top_done && bottom_done) {
	
		state=states::title;
	}

}

bool Controlador_title::loop_title(const Input& input, float delta) {

	if(input.es_input_down(Input::I_ESCAPE)) {
	
		return false;
	}

	if(input.es_input_down(Input::I_HELP)) { 
	
		solicitar_cambio_estado(Sistema_estados::estados::E_HELP);
	}
	else if(input.es_input_down(Input::I_CREDITS)) {

		solicitar_cambio_estado(Sistema_estados::estados::E_CREDITS);
	}
	else if(input.es_input_down(Input::I_DISPARAR) || input.es_input_down(Input::I_SALTAR))
	{
		solicitar_cambio_estado(Sistema_estados::estados::E_JUEGO);
	}
	
	return true;
}

void Controlador_title::dibujar(DLibV::Pantalla& pantalla)
{
	pantalla.limpiar(0, 0, 0, 255);

	DLibV::Representacion_bitmap_estatica fondo;
	fondo.establecer_textura(DLibV::Gestor_texturas::obtener(Recursos_graficos::RT_TITLEPIC));
	fondo.establecer_posicion(0, 0, 800, 600);
	fondo.establecer_recorte(0, 0, 800, 600);
	fondo.volcar(pantalla);

	switch(state) {
	
		case states::animate_logo:
		
			game_logo_top.volcar(pantalla);
			game_logo_bottom.volcar(pantalla);
		break;
		
		case states::title:

			game_logo_top.volcar(pantalla);
			game_logo_bottom.volcar(pantalla);
			
			txt_intro.volcar(pantalla);
			txt_controles.volcar(pantalla);
			txt_version.volcar(pantalla);
			txt_hi_score.volcar(pantalla);
		break;
	}
}

void Controlador_title::load_hi_scores() {

	std::stringstream ss;
	ss<<std::setfill('0')<<"== HI SCORES =="<<std::endl<<std::endl<<std::endl;

	int index=1;
	for(const auto& item : hi_scores.get()) {
	
		double integral;
						
		double frac=modf(item.game_time,&integral);
		int dec=round(frac*pow(10, 2)); //Dos decimales.
		auto seconds=fmod(integral, 60.f);
		auto minutes=floor(integral / 60.f);

		ss<<std::setw(2)<<(index++)
			<<" - LVL "<<std::setw(2)<<item.level<<" "
			<<std::setw(6)<<item.score<<" "
			<<minutes<<":"<<std::setw(2)<<seconds<<"'"<<std::setw(2)<<dec
			<<std::endl<<std::endl;
	}

	txt_hi_score.asignar(ss.str());
}
