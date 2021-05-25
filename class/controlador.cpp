#include "controlador.h"

Controlador::Controlador(DLibH::Controlador_argumentos& carg)
	:controlador_argumentos(carg),
	controlador_fps(),
	pantalla(),
	input{configuracion}//
{

}

Controlador::~Controlador()
{

}

void Controlador::inicializar()
{
	//Cargar configuración.
	configuracion.iniciar();

	//Inicializar información y sistemas según configuración.
	id_idioma=configuracion.acc_idioma();

	inicializar_entorno_grafico(pantalla, 800, 600);
	inicializar_entorno_audio();



	//Inicializar recursos.
	Cargador_recursos cargador_recursos(pantalla);
	cargador_recursos.generar_recursos_texturas();
	cargador_recursos.generar_recursos_superficies();
	cargador_recursos.generar_recursos_audio();
	cargador_recursos.generar_recursos_musica();

	//Inicializar controles.
	input.configurar();

	//Inicialización controlador tiempo.
	controlador_fps.inicializar();
}

void Controlador::inicializar_entorno_grafico(DLibV::Pantalla& pantalla, unsigned int w, unsigned int h)
{
	configurar_entorno_grafico(pantalla, w, h);
	pantalla.establecer_titulo("FAMILIAR TEMPLE");
	DLibV::Utilidades_graficas_SDL::mostrar_ocultar_cursor(false);
}

void Controlador::configurar_entorno_grafico(DLibV::Pantalla& pantalla, unsigned int w, unsigned int h)
{
	pantalla.inicializar(w, h);
	pantalla.establecer_medidas_logicas(w, h);
	pantalla.establecer_modo_ventana(DLibV::Pantalla::M_VENTANA);
}

void Controlador::intercambiar_fullscreen()
{
	unsigned int modo=pantalla.acc_modo_ventana();
	++modo;

	if(modo == DLibV::Pantalla::M_MAX_MODO)
	{
		modo=DLibV::Pantalla::M_VENTANA;
	}

LOG<<"Cambiando modo pantalla... "<<modo<<std::endl;

	pantalla.establecer_modo_ventana(modo);
}


void Controlador::inicializar_entorno_audio()
{
	LOG<<"starting audio with "<<configuracion.acc_audio_ratio()<<", "
		<<configuracion.acc_audio_salidas()<<", "
		<<configuracion.acc_audio_buffers()<<", "
		<<configuracion.acc_audio_canales()<<std::endl;

	Audio::inicializar_entorno_audio(
		configuracion.acc_audio_ratio(),
		configuracion.acc_audio_salidas(),
		configuracion.acc_audio_buffers(),
		configuracion.acc_audio_canales());

	configurar_entorno_audio();
}

void Controlador::configurar_entorno_audio()
{
	Audio::establecer_volumen(configuracion.acc_volumen_audio());
	Audio::establecer_volumen_musica(configuracion.acc_volumen_musica());
}

bool Controlador::loop(Controlador_interface& CI)
{
	//TODO: Como constante...
	const float delta=0.01;
	controlador_fps.iniciar_paso_loop();
	while(controlador_fps.consumir_loop(delta))
	{
		input.turno();

		if(input.es_input_down(Input::I_FULLSCREEN)) intercambiar_fullscreen();

		CI.preloop(input, delta);

		if(! CI.loop(input, delta))
		{
			return false;
		}

		CI.postloop();

		Audio::procesar_cola_sonido();
	}
	controlador_fps.turno();

	CI.dibujar(pantalla);

//	std::string fps="FPS:"+std::to_string(controlador_fps.acc_frames_contados())+"\n"+CI.acc_debug();
//	DLibV::Representacion_texto_auto_estatica txt(pantalla.acc_renderer(), DLibV::Gestor_superficies::obtener(Recursos_graficos::RS_FUENTE_BASE), fps);
//	txt.establecer_posicion(500, 6);
//	txt.volcar(pantalla);

	pantalla.actualizar();

	return true;
}

void Controlador::iniciar_musica()
{
	Audio::reproducir_musica(DLibA::Gestor_recursos_audio::obtener_musica(Recursos_audio::RM_MAIN));
}
