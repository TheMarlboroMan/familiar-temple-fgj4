#include "representador.h"
#include "../recursos/recursos.h"
#include <sstream>
#include <iomanip>

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


void Representador::generar_hud_municion(DLibV::Pantalla& pantalla, const std::vector<Representable *>& v)
{
	DLibV::Representacion_primitiva_caja_estatica caja_fondo=DLibV::Representacion_primitiva_caja_estatica(
		DLibH::Herramientas_SDL::nuevo_sdl_rect(0, 0, 240, 72), 0, 0, 0);
	caja_fondo.establecer_alpha(192);
	caja_fondo.volcar(pantalla);

	for(Representable * r : v)
	{
		r->transformar_bloque(bloque_transformacion);
		bloque_transformacion.rep->volcar(pantalla);
	}
}

void Representador::generar_hud_nivel(DLibV::Pantalla& pantalla, const std::string& nombre, unsigned int segundos, unsigned int decimas, unsigned int puntuacion)
{
	DLibV::Representacion_primitiva_caja_estatica caja_fondo=DLibV::Representacion_primitiva_caja_estatica(
		DLibH::Herramientas_SDL::nuevo_sdl_rect(600, 0, 200, 80), 0, 0, 0);
	caja_fondo.establecer_alpha(192);
	caja_fondo.volcar(pantalla);

	DLibV::Representacion_texto_auto_estatica nombre_nivel(pantalla.acc_renderer(), DLibV::Gestor_superficies::obtener(Recursos_graficos::RS_FUENTE_BASE), nombre);
	nombre_nivel.establecer_posicion(620, 16);
	nombre_nivel.volcar(pantalla);

	std::stringstream ss;
	ss<<std::setw(3)<<std::setfill('0')<<segundos<<"."<<std::setw(2)<<std::setfill('0')<<decimas;
	DLibV::Representacion_texto_auto_estatica txt_segundos(pantalla.acc_renderer(), DLibV::Gestor_superficies::obtener(Recursos_graficos::RS_FUENTE_BASE), ss.str()+"sec");
	txt_segundos.establecer_posicion(620, 32);
	txt_segundos.volcar(pantalla);

	ss.str(std::string());
	ss<<std::setw(6)<<std::setfill('0')<<to_string(puntuacion);
	DLibV::Representacion_texto_auto_estatica txt_puntuacion(pantalla.acc_renderer(), DLibV::Gestor_superficies::obtener(Recursos_graficos::RS_FUENTE_BASE), "["+ss.str()+"]");
	txt_puntuacion.establecer_posicion(620, 48);
	txt_puntuacion.volcar(pantalla);
}

void Representador::generar_hud_vida(DLibV::Pantalla& pantalla, unsigned int energia, unsigned int vidas, const Frame_sprites& fs_energia, const Frame_sprites& fs_vidas)
{
	unsigned int i=0;

	//Energía...
	bloque_transformacion.establecer_tipo(Bloque_transformacion_representable::TR_BITMAP);
	bloque_transformacion.establecer_alpha(192);
	bloque_transformacion.establecer_recurso(Recursos_graficos::RT_HUD);

	while(i < energia)
	{
		bloque_transformacion.establecer_recorte(fs_energia.x, fs_energia.y, fs_energia.w, fs_energia.h);
		bloque_transformacion.establecer_posicion(16+( i*(fs_energia.w+2)), 48, fs_energia.w, fs_energia.h);
		bloque_transformacion.rep->volcar(pantalla);
		++i;
	}

	//Vidas...
	bloque_transformacion.establecer_recurso(Recursos_graficos::RT_HUD);
	bloque_transformacion.establecer_recorte(fs_vidas.x, fs_vidas.y, fs_vidas.w, fs_vidas.h);
	bloque_transformacion.establecer_posicion(16, 32, fs_vidas.w, fs_vidas.h);
	bloque_transformacion.rep->volcar(pantalla);

	DLibV::Representacion_texto_auto_estatica txt_vidas(pantalla.acc_renderer(), DLibV::Gestor_superficies::obtener(Recursos_graficos::RS_FUENTE_BASE), " x "+to_string(vidas));
	txt_vidas.establecer_posicion(32, 32);
	txt_vidas.volcar(pantalla);
}

void Representador::generar_hud_ankh(DLibV::Pantalla& pantalla, unsigned int actuales, unsigned int en_nivel, unsigned int total, const Frame_sprites& fs)
{
	if(!en_nivel) return;

	bloque_transformacion.establecer_tipo(Bloque_transformacion_representable::TR_BITMAP);
	bloque_transformacion.establecer_alpha(192);
	bloque_transformacion.establecer_recurso(Recursos_graficos::RT_HUD);
	bloque_transformacion.establecer_recorte(fs.x, fs.y, fs.w, fs.h);
	bloque_transformacion.establecer_posicion(80, 32, fs.w, fs.h);
	bloque_transformacion.rep->volcar(pantalla);

	DLibV::Representacion_texto_auto_estatica txt_ankh(pantalla.acc_renderer(), DLibV::Gestor_superficies::obtener(Recursos_graficos::RS_FUENTE_BASE), " x "+to_string(actuales)+"/"+to_string(en_nivel)+" ["+to_string(total)+"]");
	txt_ankh.establecer_posicion(92, 32);
	txt_ankh.volcar(pantalla);
}

void Representador::focus_hud(
	DLibV::Pantalla& _screen, 
	unsigned int _focus_meter, 
	powers::power_type _current_power
) {

	std::stringstream ss{};

	switch(_current_power) {

		case powers::power_type::time: ss<<"time "; break;
		case powers::power_type::fire: ss<<"fire "; break;
		case powers::power_type::ammo: ss<<"ammo "; break;
	}

	ss<<_focus_meter;

	DLibV::Representacion_texto_auto_estatica txt(_screen.acc_renderer(), DLibV::Gestor_superficies::obtener(Recursos_graficos::RS_FUENTE_BASE), ss.str());
	txt.establecer_posicion(620, 58);
	txt.volcar(_screen);

}
