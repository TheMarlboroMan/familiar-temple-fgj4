#ifndef CONTROLADOR_JUEGO_H
#define CONTROLADOR_JUEGO_H

#include "controlador_interface.h"
#include "../herramientas_proyecto/generador_numeros.h"
#include "../proyectil.h"
#include "../graficos/representador.h"
#include "../cargadores/cargador_mapas.h"
#include "../jugador.h"
#include "../sistemas/control_armas.h"
#include "../sistemas/sistema_puntuacion.h"
#include "../sistemas/sistema_vidas.h"

#include <fstream>
#include <queue>

extern DLibH::Log_base LOG;

class Controlador_juego:public Controlador_interface
{
	private:

	struct Evento_juego
	{
		enum class tipos {SALIDA_NIVEL, PERDER_VIDA_POR_CELDA, PERDER_VIDA_POR_ENEMIGO, PERDER_VIDA_POR_TIEMPO};
		tipos tipo;
		Evento_juego(tipos t):tipo(t) {}
	};

	class Fade
	{
		public:

		enum class tcallback {REINICIAR_NIVEL, VOLVER_TITULO, GAME_OVER, FIN_1, FIN_2};

		private:

		unsigned int r, g, b;
		bool activo;
		tcallback tipo;
		float alpha;

		public:

		Fade(): r(0), g(0), b(0), activo(false),
			tipo(tcallback::REINICIAR_NIVEL), alpha(0.0)
		{}

		unsigned int acc_r() const {return r;}
		unsigned int acc_g() const {return g;}
		unsigned int acc_b() const {return b;}
		unsigned int acc_alpha() const {return alpha;}
		tcallback acc_tipo() const {return tipo;}

		void activar(tcallback t, unsigned int pr, unsigned int pg, unsigned int pb)
		{
			tipo=t;
			r=pr;
			g=pg;
			b=pb;
			alpha=0.0;
			activo=true;
LOG<<"ACTIVANDO FADE"<<std::endl;
		}

		void finalizar() {activo=false;}
		bool es_activo() const {return activo;}
		bool es_finalizado() const {return alpha >= 255.0;}

		void turno(float delta)
		{
			alpha+=(delta * 1.000) * 255;
			if(alpha > 255.0) alpha=255.0;
		}		
	};

	Fade fade;
	Sistema_puntuacion sistema_puntuacion;
	Sistema_vidas sistema_vidas;
	Control_armas control_armas;
	Cargador_mapas cargador_mapas;
	Mapa mapa;
	DLibV::Camara camara;
	Representador representador;
	Jugador jugador;
	std::queue<Evento_juego> cola_eventos;
	std::vector<std::unique_ptr<Proyectil >> proyectiles;
	std::vector<std::unique_ptr<Proyectil >> proyectiles_enemigos;
	unsigned int nivel_actual;
	float segundos_restantes;

	void cola_sonido(unsigned int sonido, unsigned int vol);
	void importar_nivel(unsigned int indice_nivel);
	Input_usuario recoger_input_usuario(const Input& input);
	void procesar_input_jugador(const Input& input, float delta);
	void logica_jugador(float delta);

	void logica_nivel();
	void turno_nivel(float delta);
	void reiniciar_nivel();

	void sumar_puntuacion(unsigned int);

	void procesar_cola_eventos_juego();
	void evaluar_eventos_juego();
	void evaluar_enfoque_camara();

	void generar_disparo_jugador();
	void generar_disparo_enemigo(const Disparador& e);
	void generar_disparo_boss(const Boss& e);

	void herir_jugador();
	void iniciar_fin_juego();
	void procesar_callback_fade();

	virtual void dibujar_juego(DLibV::Pantalla& pantalla);
	virtual void dibujar_hud(DLibV::Pantalla& pantalla);

	public:

	Controlador_juego(const DLibH::Controlador_argumentos& carg, Sistema_estados& s);

	virtual void preloop(const Input& input, float delta)
	{

	}

	virtual void postloop()
	{

	}

	virtual bool loop(const Input& input, float delta);
	virtual void dibujar(DLibV::Pantalla& pantalla);
	
	void reset();

};

#endif
