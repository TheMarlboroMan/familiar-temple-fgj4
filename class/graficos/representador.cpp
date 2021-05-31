#include "representador.h"
#include "../recursos/recursos.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

#ifdef WINCOMPIL
	using namespace parche_mingw;
#else
	using namespace std;
#endif

Representador::Representador()
{

}

Representador::~Representador()
{

}

unsigned int Representador::generar_vista(
	DLibV::Pantalla& pantalla,
	DLibV::Camara& camara,
	const std::vector<Representable *>& v)
{
	unsigned int total=0;

	for(Representable * r : v)
	{
		r->transformar_bloque(bloque_transformacion);
		if(bloque_transformacion.rep->volcar(pantalla, camara)) ++total;
	}

	return total;
}

void Representador::reload_data() {

	std::ifstream file("/home/daniel/Desktop/size.txt");	
	file
>>left_x
>>left_y
>>left_w
>>left_h
>>center_x
>>center_y
>>center_w
>>center_h
>>ammo_x
>>ammo_y
>>ammo_reserve_x
>>ammo_reserve_y
>>focus_bar_x
>>focus_bar_y
>>focus_bar_w
>>focus_bar_h
>>focus_power_x
>>focus_power_y
>>health_x
>>health_y
>>health_margin
>>timer_x
>>timer_y
>>ankh_icon_x
>>ankh_icon_y
>>ankh_x
>>ankh_y
>>score_x
>>score_y
>>level_name_box_x
>>level_name_box_y
>>level_name_box_w
>>level_name_box_h
>>level_name_x
>>level_name_y
>>lives_icon_x
>>lives_icon_y
>>lives_x
>>lives_y;
}

void Representador::hud_overlay(
	DLibV::Pantalla& _screen,
	bool _level_name_box
) {

	auto draw_box=[&](int x, int y, int w, int h) {
	
		DLibV::Representacion_primitiva_caja_estatica box=DLibV::Representacion_primitiva_caja_estatica(
			DLibH::Herramientas_SDL::nuevo_sdl_rect(x, y, w, h), 
			32, 32, 32
		);
		
		box.establecer_alpha(128);
		box.volcar(_screen);
	};
	
	//left
	draw_box(left_x, left_y, left_w, left_h);
	
	//timer and ankhs
	draw_box(center_x, center_y, center_w, center_h);
	
	if(_level_name_box) {
	
		draw_box(level_name_box_x, level_name_box_y, level_name_box_w, level_name_box_h);
	}
}

void Representador::hud_ammo(
	DLibV::Pantalla& _screen, 
	const std::vector<Representable *>& _v,
	int _remaining
) {

	for(Representable * r : _v) {
	
		r->transformar_bloque(bloque_transformacion);
		
		auto pos=bloque_transformacion.acc_posicion();
		pos.x+=ammo_x;
		pos.y+=ammo_y;
		
		bloque_transformacion.establecer_posicion(pos.x, pos.y, pos.w, pos.h);
		bloque_transformacion.rep->volcar(_screen);
	}
	
	std::stringstream ss;
	ss<<std::setw(3)<<std::setfill('0')<<_remaining;
	
	DLibV::Representacion_texto_auto_estatica txt(
		_screen.acc_renderer(), 
		DLibV::Gestor_superficies::obtener(Recursos_graficos::RS_FUENTE_BASE), 
		ss.str()
	);
	txt.establecer_posicion(ammo_reserve_x, ammo_reserve_y);
	txt.volcar(_screen);
}

void Representador::hud_focus(
	DLibV::Pantalla& _screen, 
	int _focus, 
	const Frame_sprites& _power,
	bool _switches_shortly
) {

	//Focus bar...
	DLibV::Representacion_primitiva_caja_estatica bar_bg{
		DLibH::Herramientas_SDL::nuevo_sdl_rect(focus_bar_x, focus_bar_y, focus_bar_w, focus_bar_h), 
		32, 120, 157
	};
		
	bar_bg.establecer_alpha(255);
	bar_bg.volcar(_screen);
	
	int bar_w=(_focus * focus_bar_w) / 50;
	
	Uint8 fg_r=_focus==50 ? 63 : 42,
		fg_g=_focus==50 ? 152 : 149,
		fg_b=_focus==50 ? 84 : 193;
	
	DLibV::Representacion_primitiva_caja_estatica bar_fg{
		DLibH::Herramientas_SDL::nuevo_sdl_rect(focus_bar_x, focus_bar_y, bar_w, focus_bar_h), 
		fg_r, fg_g, fg_b
	};
	
	
	bar_fg.establecer_alpha(255);
	bar_fg.volcar(_screen);
	
	//Power icon...
	bloque_transformacion.establecer_tipo(Bloque_transformacion_representable::TR_BITMAP);
	bloque_transformacion.establecer_alpha(_switches_shortly ? 64 : 255);
	bloque_transformacion.establecer_blend(DLibV::Representacion::BLEND_ALPHA);
	bloque_transformacion.establecer_recurso(Recursos_graficos::RT_HUD);
	bloque_transformacion.establecer_recorte(_power.x, _power.y, _power.w, _power.h);
	bloque_transformacion.establecer_posicion(focus_power_x, focus_power_y, _power.w, _power.h);
	bloque_transformacion.rep->volcar(_screen);
}

void Representador::hud_health(
	DLibV::Pantalla& _screen, 
	int _health, 
	const Frame_sprites& _full, 
	const Frame_sprites& _empty
) {

	int max_draw=std::max(_health, 3);	
	bloque_transformacion.establecer_tipo(Bloque_transformacion_representable::TR_BITMAP);
	bloque_transformacion.establecer_alpha(255);
	bloque_transformacion.establecer_blend(DLibV::Representacion::BLEND_ALPHA);
	bloque_transformacion.establecer_recurso(Recursos_graficos::RT_HUD);

	int i=0;
	while(i < max_draw) {
	
		const Frame_sprites * fs=_health <= i 
			? &_empty
			: &_full;
			
		int xpos=i*health_margin;
			
		bloque_transformacion.establecer_recorte(fs->x, fs->y, fs->w, fs->h);
		bloque_transformacion.establecer_posicion(
			health_x+xpos,
			health_y, fs->w, fs->h
		);
		bloque_transformacion.rep->volcar(_screen);
		i++;
	}
}

void Representador::hud_timer(
	DLibV::Pantalla& _screen, 
	float _seconds,
	bool alert
) {
	double seconds{0.};
	double frac=modf(_seconds,&seconds);
	int dec=round(frac*pow(10, 2)); //Dos decimales.
	
	std::stringstream ss;
	ss<<std::setw(3)<<std::setfill('0')<<seconds<<"."<<std::setw(2)<<std::setfill('0')<<dec;
	
	DLibV::Representacion_texto_auto_estatica txt{
		_screen.acc_renderer(), 
		DLibV::Gestor_superficies::obtener(
			alert ? Recursos_graficos::RS_FONT_RED : Recursos_graficos::RS_FUENTE_BASE			
		), ss.str()
	};
	
	txt.establecer_posicion(timer_x, timer_y);
	txt.volcar(_screen);
}

void Representador::hud_ankh(
	DLibV::Pantalla& _screen, 
	unsigned int _held, 
	unsigned int _level, 
	const Frame_sprites& _fs
) {
	if(!_level) {
		return;
	}

	bloque_transformacion.establecer_tipo(Bloque_transformacion_representable::TR_BITMAP);
	bloque_transformacion.establecer_alpha(192);
	bloque_transformacion.establecer_blend(DLibV::Representacion::BLEND_ALPHA);
	bloque_transformacion.establecer_recurso(Recursos_graficos::RT_HUD);
	bloque_transformacion.establecer_recorte(_fs.x, _fs.y, _fs.w, _fs.h);
	bloque_transformacion.establecer_posicion(ankh_icon_x, ankh_icon_y, _fs.w, _fs.h);
	bloque_transformacion.rep->volcar(_screen);

	std::stringstream ss;
	ss<<_held<<"/"<<_level;

	DLibV::Representacion_texto_auto_estatica txt{
		_screen.acc_renderer(), 
		DLibV::Gestor_superficies::obtener(
			_held==_level 
				? Recursos_graficos::RS_FUENTE_BASE
				: Recursos_graficos::RS_FONT_RED
			), 
		ss.str()
	};
	txt.establecer_posicion(ankh_x, ankh_y);
	txt.volcar(_screen);
}

void Representador::hud_score(
	DLibV::Pantalla& _screen, 
	unsigned int _score
) {

	std::stringstream ss;
	ss<<std::setw(6)<<std::setfill('0')<<to_string(_score);
	DLibV::Representacion_texto_auto_estatica txt(
		_screen.acc_renderer(), 
		DLibV::Gestor_superficies::obtener(Recursos_graficos::RS_FUENTE_BASE), 
		ss.str()
	);
	txt.establecer_posicion(score_x, score_y);
	txt.volcar(_screen);
}

void Representador::hud_level_name(
	DLibV::Pantalla& _screen, 
	const std::string& _level, 
	int _lvl_number,
	unsigned int _lives, 
	const Frame_sprites& _fs
) {
	std::stringstream ss;
	ss<<"Now entering level "<<_lvl_number<<": "<<_level;
	
	DLibV::Representacion_texto_auto_estatica txt(
		_screen.acc_renderer(), 
		DLibV::Gestor_superficies::obtener(Recursos_graficos::RS_FONT_LARGE), 
		ss.str()
	);
	txt.establecer_posicion(level_name_x, level_name_y);
	txt.volcar(_screen);
	
	//Lives...
	bloque_transformacion.establecer_tipo(Bloque_transformacion_representable::TR_BITMAP);
	bloque_transformacion.establecer_alpha(255);
	bloque_transformacion.establecer_blend(DLibV::Representacion::BLEND_ALPHA);
	bloque_transformacion.establecer_recurso(Recursos_graficos::RT_HUD);
	bloque_transformacion.establecer_recorte(_fs.x, _fs.y, _fs.w, _fs.h);
	bloque_transformacion.establecer_posicion(lives_icon_x, lives_icon_y, _fs.w, _fs.h);
	bloque_transformacion.rep->volcar(_screen);
	
	ss.str("");
	ss<<"x "<<_lives;
	DLibV::Representacion_texto_auto_estatica txt_lives(
		_screen.acc_renderer(), 
		DLibV::Gestor_superficies::obtener(Recursos_graficos::RS_FUENTE_BASE), 
		ss.str()
	);
	txt_lives.establecer_posicion(lives_x, lives_y);
	txt_lives.volcar(_screen);
	
}

void Representador::power_overlay(
	DLibV::Pantalla& _screen, 
	bool _active, 
	powers::power_type _type,
	const Frame_sprites& fs_fire,
	const Frame_sprites& fs_time
) {

	if(!_active) {
	
		return;
	}
	
	const Frame_sprites* fs=nullptr;
	if(_type==powers::power_type::fire) {
		
		fs=&fs_fire;
	}
	else if(_type==powers::power_type::time) {
		
		fs=&fs_time;
	}
		
	if(nullptr==fs) {
	
		return;
	}
	
	bloque_transformacion.establecer_tipo(Bloque_transformacion_representable::TR_BITMAP);
	bloque_transformacion.establecer_alpha(255);
	bloque_transformacion.establecer_blend(DLibV::Representacion::BLEND_ALPHA);
	bloque_transformacion.establecer_recurso(Recursos_graficos::RT_HUD);
	bloque_transformacion.establecer_recorte(fs->x, fs->y, fs->w, fs->h);
	
	auto screen_h=_screen.acc_simulacro_caja().h,
		screen_w=_screen.acc_simulacro_caja().w;
	
	//left
	bloque_transformacion.establecer_posicion(0, 0, fs->w*2, screen_h);
	bloque_transformacion.rep->volcar(_screen);
	
	//right
	bloque_transformacion.rotar(180);
	bloque_transformacion.establecer_posicion(screen_w-(fs->w*2), 0, fs->w*2, screen_h);	
	bloque_transformacion.rep->volcar(_screen);
}
