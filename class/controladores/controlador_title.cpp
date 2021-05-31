#include "controlador_title.h"
#include "../env.h"

#include <sstream>
#include <iomanip>

const std::string Controlador_title::cadena_intro="F A M I L I A R    T E M P L E\n\n\n\n"
"The evil priest Nebneteru has returned to Earth once more.\n\nVenture inside the evil temple and defeat him before he regains his power!.";
const std::string Controlador_title::cadena_controles="Press F1 for help\n\nPress JUMP or FIRE to play.";
const std::string Controlador_title::cadena_creditos="A \"game\" by The Marlboro Man.\n\nMusic: \"temple of the mystics\" by jdagenet.\n\nTileset: \"hieroglyph sprites\" by Andor Salga.\n\nSound effects by Diode111 and Kayden Riggs.\n\nMade for the 4th Familiar Game Jam.\n\n\n";


Controlador_title::Controlador_title(
	Sistema_estados& s, 
	hi_score_manager& _hi_score,
	DLibV::Pantalla& pantalla
)
	:Controlador_interface(s),
	hi_scores{_hi_score},
	txt_intro(pantalla.acc_renderer(), DLibV::Gestor_superficies::obtener(Recursos_graficos::RS_FUENTE_BASE), cadena_intro),
	txt_controles(pantalla.acc_renderer(), DLibV::Gestor_superficies::obtener(Recursos_graficos::RS_FUENTE_BASE), cadena_controles),
	txt_creditos(pantalla.acc_renderer(), DLibV::Gestor_superficies::obtener(Recursos_graficos::RS_FUENTE_BASE),
		cadena_creditos+std::string{"2015, v"}+std::to_string(env::major_version)+"."
		+std::to_string(env::minor_version)+"."
		+std::to_string(env::patch_version)),
	txt_hi_score(pantalla.acc_renderer(), DLibV::Gestor_superficies::obtener(Recursos_graficos::RS_FUENTE_BASE), "")
{

	txt_intro.establecer_posicion(32, 32);
	txt_controles.establecer_posicion(32, 160);
	txt_creditos.establecer_posicion(420, 480);
	txt_hi_score.establecer_posicion(32, 200);
	load_hi_scores();
}

bool Controlador_title::loop(const Input& input, float delta)
{
	if(input.es_senal_salida() || input.es_input_down(Input::I_ESCAPE))
	{
		return false;
	}

	if(input.es_input_down(Input::I_HELP)) {

		solicitar_cambio_estado(Sistema_estados::estados::E_HELP);
	}

	if(input.es_input_down(Input::I_DISPARAR) || input.es_input_down(Input::I_SALTAR))
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

	txt_intro.volcar(pantalla);
	txt_controles.volcar(pantalla);
	txt_creditos.volcar(pantalla);
	txt_hi_score.volcar(pantalla);
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
