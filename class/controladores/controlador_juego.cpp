#include "controlador_juego.h"
#include <cassert>
#include "../env.h"

#ifdef WINCOMPIL
	using namespace parche_mingw;
#else
	using namespace std;
#endif

Controlador_juego::Controlador_juego(const DLibH::Controlador_argumentos& CARG, Sistema_estados& s)
	:Controlador_interface(s),
	cargador_mapas(env::make_data_path("data/recursos/niveles.txt")),
	nivel_actual(1), segundos_restantes(0.0)
{
	camara=DLibV::Camara(0, 0, 800, 600);

	int pos_nivel=CARG.buscar("-n");

	if(pos_nivel!=-1)
	{
		if(pos_nivel != -1) nivel_actual=std::atoi(CARG.acc_argumento(pos_nivel+1).c_str());
	}

	importar_nivel(nivel_actual);
}

bool Controlador_juego::loop(const Input& input, float delta)
{
	if( (input.es_senal_salida() || input.es_input_down(Input::I_ESCAPE)) && !fade.es_activo())
	{
		fade.activar(Fade::tcallback::VOLVER_TITULO, 0, 0, 0);
	}
	else
	{
		if(!fade.es_activo())
		{
			logica_nivel();
			turno_nivel(delta);
			procesar_input_jugador(input, delta);
			logica_jugador(delta);
			evaluar_eventos_juego();
			procesar_cola_eventos_juego();
		}
		else
		{
			fade.turno(delta);
			if(fade.es_finalizado())
			{
				procesar_callback_fade();
			}
		}
	}

	return true;
}


void Controlador_juego::procesar_callback_fade()
{
	switch(fade.acc_tipo())
	{
		case Fade::tcallback::VOLVER_TITULO:
			reset();
			solicitar_cambio_estado(Sistema_estados::estados::E_TITLE);
		break;

		case Fade::tcallback::REINICIAR_NIVEL:
			LOG<<"PROCESANDO CALLBACK REINICIAR NIVEL"<<std::endl;
			importar_nivel(nivel_actual); //Lol.
			jugador.reiniciar_energia();
			control_armas.reset();
		break;

		case Fade::tcallback::GAME_OVER:
			reset();
			solicitar_cambio_estado(Sistema_estados::estados::E_GAME_OVER);
		break;

		case Fade::tcallback::FIN_1:
			reset();
			solicitar_cambio_estado(Sistema_estados::estados::E_FIN_1);
		break;

		case Fade::tcallback::FIN_2:
			reset();
			solicitar_cambio_estado(Sistema_estados::estados::E_FIN_2);
		break;
	}

	fade.finalizar();
}

void Controlador_juego::procesar_input_jugador(const Input& input, float delta)
{
	Input_usuario iu=recoger_input_usuario(input);
	jugador.recibir_input(iu);

	//Esto va justo aquí, porque puede comportar un cambio de estado.
	jugador.turno(delta);

	//Según el estado actual y el input, ¿podemos recalcular el estado?.
	//Tenemos este visitante aquí, que lo usaremos para que no se nos pase
	//ni un sólo estado. Podríamos hacer un switch case, pero me gusta la
	//idea de que si añadimos estados nuevos no se van a pasar de largo.

	class Vis:public Visitante_estado_jugador
	{
		private:

		const Jugador& jugador;
		Input_usuario& iu;
//		Nivel& nivel;

		bool intentar_disparar;
		bool recargar;
		bool saltar;

		public:

		bool es_intentar_disparar() const {return intentar_disparar;}
		bool es_recargar() const {return recargar;}
		bool es_saltar() const {return saltar;}

		Vis(Jugador& pj, Input_usuario& pi): //, Nivel& pn):
			jugador(pj), iu(pi), //nivel(pn),
			intentar_disparar(false), recargar(false), saltar(false)
		{}

		virtual void visitar(Estado_jugador_suelo& e)
		{
			if(e.es_recargar()) recargar=true;
			else if(iu.saltar && jugador.puede_saltar()) saltar=true;
			else if(iu.disparar) intentar_disparar=true;
		}

		virtual void visitar(Estado_jugador_recargar&) {}
		virtual void visitar(Estado_jugador_aire&) {}

	}vis(jugador, iu); //, mapa.acc_nivel());

	//Recibimos el visitante. Inspeccionamos si aplicamos los casos que necesitan más información,
	//como bajar o subir de escaleras o agacharse, que no es posible si estamos cerca de una escalera.

	jugador.recibir_visitante_estado(vis);

	//Finalmente, según el estado final, procesar el input.
#ifndef NDEBUG
	Estado_jugador::t_estados copia_estado=jugador.obtener_estado();
#endif
	jugador.procesar_estado(delta);
#ifndef NDEBUG
	assert(jugador.obtener_estado()==copia_estado);
#endif

	if(vis.es_intentar_disparar())
	{
		if(control_armas.puede_disparar())
		{
			generar_disparo_jugador();
			control_armas.disparar();
		}
		else
		{
			if(!control_armas.acc_municion_actual()) cola_sonido(Recursos_audio::RS_ARMA_VACIA, 128);
		}
	}

	if(vis.es_saltar())
	{
		jugador.saltar();
		cola_sonido(Recursos_audio::RS_SALTO, 128);
	}

	if(vis.es_recargar())
	{
		if(!control_armas.es_arma_llena())
		{
			jugador.recargar();
			control_armas.recargar();
			cola_sonido(Recursos_audio::RS_RECARGAR, 128);
		}
	}
}

void Controlador_juego::importar_nivel(unsigned int indice_nivel)
{
	if(mapa.es_valido())
	{
		proyectiles.clear();
		proyectiles_enemigos.clear();
	}

	try
	{
		sistema_puntuacion.reset_ankh_nivel();

		//Cargar nivel.
		mapa=cargador_mapas.fabricar_mapa_indice(indice_nivel);

		//Obtener del nivel las coordenadas de la celda de inicio...
		const Nivel::Coordenadas inicio=mapa.obtener_coordenadas_inicio_en_rejilla();

		jugador.establecer_posicion(inicio.x * Celda::DIM_CELDA, inicio.y * Celda::DIM_CELDA);

		segundos_restantes=mapa.acc_segundos_finalizar();

		int lim_nivel_w=mapa.acc_nivel().acc_w_en_celdas()*Celda::DIM_CELDA;
		int lim_nivel_h=mapa.acc_nivel().acc_h_en_celdas()*Celda::DIM_CELDA;

		if(lim_nivel_w <= camara.acc_w() || lim_nivel_h <= camara.acc_h())
		{
			camara.limpiar_limite();
		}
		else
		{
			camara.establecer_limites(0,0, lim_nivel_w, lim_nivel_h);
		}


		nivel_actual=indice_nivel;
	}
	catch(Excepcion_cargador_mapas &e)
	{
		LOG<<"ERROR: No ha sido posible cargar el nivel el en indice "<<indice_nivel<<std::endl;
		std::exit(0);
	}
	catch(Excepcion_mapa &e)
	{
		LOG<<"ERROR: No se localiza la entrada en el nivel "<<indice_nivel<<std::endl;
		std::exit(0);
	}
}

void Controlador_juego::reset()
{
	LOG<<"Reset juego"<<std::endl;

	nivel_actual=1;

	sistema_vidas.reset();
	sistema_puntuacion.reset();
	control_armas.reset();

	proyectiles.clear();
	proyectiles_enemigos.clear();

	importar_nivel(nivel_actual);
}

void Controlador_juego::reiniciar_nivel()
{
	LOG<<"RESTANDO VIDA"<<std::endl;
	sistema_vidas.restar_vida();
	cola_sonido(Recursos_audio::RS_PERDER_VIDA, 128);

	if(sistema_vidas.es_quedan_vidas())
	{
		LOG<<"Activando reinicio nivel"<<std::endl;
		fade.activar(Fade::tcallback::REINICIAR_NIVEL, 0, 0, 0);
	}
	else
	{
		LOG<<"Invocando game over."<<std::endl;
		fade.activar(Fade::tcallback::GAME_OVER, 128, 0, 0);
	}
}

Input_usuario Controlador_juego::recoger_input_usuario(const Input& input)
{
	Input_usuario iu;

	if(input.es_input_pulsado(Input::I_DERECHA)) iu.mov_horizontal=1;
	else if(input.es_input_pulsado(Input::I_IZQUIERDA)) iu.mov_horizontal=-1;
	else if(input.es_input_up(Input::I_DERECHA)) iu.keyup_horizontal=1;
	else if(input.es_input_up(Input::I_IZQUIERDA)) iu.keyup_horizontal=-1;

	if(input.es_input_pulsado(Input::I_ARRIBA)) iu.mov_vertical=-1;
	else if(input.es_input_pulsado(Input::I_ABAJO)) iu.mov_vertical=1;

	if(input.es_input_down(Input::I_SALTAR)) iu.saltar=true;
	if(input.es_input_down(Input::I_DISPARAR)) iu.disparar=true;
	if(input.es_input_pulsado(Input::I_APUNTAR)) iu.apuntar=true;
	if(input.es_input_down(Input::I_APUNTAR)) iu.keydown_apuntar=true;

	if(input.es_input_down(Input::I_RECARGAR)) iu.recargar=true;

	return iu;
}

/*La lógica del jugador es la que genera realmente el movimiento. Junto con
la generación del movimiento cabe el caso borderline de que se cambie también
el estado (por ejemplo, se ha terminado el impulso).*/

void Controlador_juego::logica_jugador(float delta)
{
	class Vis:public Visitante_estado_jugador
	{
		private:

		unsigned int flags;
		const unsigned int MOVIMIENTO_PERMITIDO=1;
		const unsigned int CAMBIO_DIRECCION_PERMITIDA=2;

		public:

		Vis(): flags(0) {}

		bool es_movimiento_permitido() const {return flags & MOVIMIENTO_PERMITIDO;}
		bool es_cambio_direccion_permitida() const {return flags & CAMBIO_DIRECCION_PERMITIDA;}

		virtual void visitar(Estado_jugador_suelo&)			{flags=MOVIMIENTO_PERMITIDO | CAMBIO_DIRECCION_PERMITIDA;}
		virtual void visitar(Estado_jugador_aire&)			{flags=MOVIMIENTO_PERMITIDO;}
		virtual void visitar(Estado_jugador_recargar&)			{flags=MOVIMIENTO_PERMITIDO | CAMBIO_DIRECCION_PERMITIDA;;}
	}vis;

	jugador.recibir_visitante_estado(vis);

	//Calcular hacia dónde mira.
	if(vis.es_cambio_direccion_permitida() && !jugador.es_apuntando())
	{
		jugador.recalcular_direccion();
	}

	//Movimiento del jugador... primero un eje y luego el otro.
	if(vis.es_movimiento_permitido())
	{
		auto celdas_nueva_posicion=[this](const Espaciable::t_caja& caja)
		{
			std::vector<const Celda *> colisiones=mapa.acc_nivel().celdas_para_caja(caja);
			auto it=std::remove_if(colisiones.begin(), colisiones.end(), [this](const Celda * c) {return !c->es_solida();});
			colisiones.erase(it, colisiones.end());
			return colisiones;
		};

		auto v=jugador.acc_vector();
		auto caja=jugador.copia_caja();

		//Colisión con celdas...

		if(v.x)
		{
			jugador.desplazar_caja(v.x * delta, 0.0);
			auto nuevasx=celdas_nueva_posicion(jugador.copia_caja());
			if(nuevasx.size())
			{
				auto cc=nuevasx[0]->copia_caja();
				if(v.x > 0.0) jugador.chocar_desde_izquierda_con(cc);
				else jugador.chocar_desde_derecha_con(cc);
			}
		}

		caja=jugador.copia_caja();
		if(v.y)
		{
			jugador.desplazar_caja(0.0, v.y * delta);
			auto nuevasy=celdas_nueva_posicion(jugador.copia_caja());
			if(nuevasy.size())
			{
				auto cc=nuevasy[0]->copia_caja();
				if(v.y > 0.0) jugador.chocar_desde_arriba_con(cc);
				else jugador.chocar_desde_abajo_con(cc);
			}
		}
	}
}

void Controlador_juego::procesar_cola_eventos_juego()
{
	while(cola_eventos.size())
	{
		Evento_juego e=cola_eventos.front();

		switch(e.tipo)
		{
			case Evento_juego::tipos::SALIDA_NIVEL:
				if(sistema_puntuacion.acc_ankh_nivel()==mapa.acc_ankh_nivel())
				{
					sistema_puntuacion.marcar_nivel_como_todos_ankh();
					LOG<<"Finalizado nivel "<<nivel_actual<<" con todos ANKH"<<std::endl;
				}
				importar_nivel(nivel_actual+1);
				while(cola_eventos.size()) cola_eventos.pop();
			break;

			case Evento_juego::tipos::PERDER_VIDA_POR_TIEMPO:
			case Evento_juego::tipos::PERDER_VIDA_POR_CELDA:
			case Evento_juego::tipos::PERDER_VIDA_POR_ENEMIGO:
				LOG<<"Detectado reinicio de nivel por perdida de vida"<<std::endl;
				reiniciar_nivel();
				while(cola_eventos.size()) cola_eventos.pop();
			break;
		}

		if(cola_eventos.size()) cola_eventos.pop();
	}
}

void Controlador_juego::evaluar_eventos_juego()
{
	//Evaluar si está en una única celda y es la salida...
	auto colisiones=mapa.acc_nivel().celdas_para_caja(jugador.copia_caja());

	if(segundos_restantes < 0.0)
	{
		LOG<<"Insertando evento fin tiempo"<<std::endl;
		cola_eventos.push(Evento_juego(Evento_juego::tipos::PERDER_VIDA_POR_TIEMPO));
		return;
	}

	if(colisiones.size())
	{
		if(colisiones.size()==1 && colisiones[0]->acc_tipo()==Celda::T_SALIDA)
		{
			LOG<<"Insertando evento salida nivel"<<std::endl;
			cola_eventos.push(Evento_juego(Evento_juego::tipos::SALIDA_NIVEL));
			return;
		}

		//Localizar las letales. Sobre las letales, comprobar si una caja un poco más pequeña está dentro.
		if(!jugador.es_en_aire())
		{
			auto total_letales=std::count_if(colisiones.begin(), colisiones.end(), [](const Celda * const c) {return c->es_letal();});

			if(total_letales)
			{
				auto caja=jugador.copia_caja();
				caja.origen.x+=4; caja.origen.y+=4; caja.w-=8; caja.h-=8;

				if(std::any_of(colisiones.begin(), colisiones.end(), [caja](const Celda * const c)
					{
						return c->es_letal() &&
						caja.es_en_colision_con(c->copia_caja());
					} ) )
				{
					LOG<<"Insertando evento perder vida por celda"<<std::endl;
					cola_eventos.push(Evento_juego(Evento_juego::tipos::PERDER_VIDA_POR_CELDA));
					return;
				}
			}
		}
	}

	//Comprobar colisiones con enemigos...
	auto& enemigos=mapa.acc_enemigos();
	for(auto& e : enemigos)
	{
		if(e->en_colision_con(jugador) && !jugador.es_invulnerable())
		{
			if(e->es_saltable() && jugador.es_en_aire()) continue;
			else
			{
				herir_jugador();
			}
		}
	}

	//Y comprobarlas con proyectiles enemigos...
	if(!jugador.es_en_aire())
	{
		for(auto& p : proyectiles_enemigos)
		{
			if(p->en_colision_con(jugador) && !jugador.es_invulnerable())
			{
				p->marcar_para_borrar();
				herir_jugador();
			}
		}
	}

	//Comprobar colisiones con items...
	class Vis:public Visitante_item
	{
		private:

		Control_armas& ca;
		bool recoge_ankh;
		bool recoge_salud;

		void recoger_arma(Control_armas::t_armas t, unsigned int c)
		{
			ca.mut_arma_actual(t);
			ca.sumar_reserva(t, c);
			ca.rellenar();
		}

		public:

		Vis(Control_armas& c):ca(c), recoge_ankh(false), recoge_salud(false) {}

		void reset() {recoge_ankh=false; recoge_salud=false;}

		bool es_recoge_ankh() const {return recoge_ankh;}
		bool es_recoge_salud() const {return recoge_salud;}

		virtual void visitar(Item_base&) {}
		virtual void visitar(Item_pistola& i) {recoger_arma(Control_armas::t_armas::PISTOLA, 24);}
		virtual void visitar(Item_revolver& i) {recoger_arma(Control_armas::t_armas::REVOLVER, 12);}
		virtual void visitar(Item_escopeta& i) {recoger_arma(Control_armas::t_armas::ESCOPETA, 8);}
		virtual void visitar(Item_ankh& i) {recoge_ankh=true;}
		virtual void visitar(Item_salud& i) {recoge_salud=true;}

	}vis(control_armas);

	if(!jugador.es_en_aire())
	{
		bool sonar=false;
		auto& items=mapa.acc_items();
		for(auto& e : items)
		{
			if(jugador.en_colision_con(*e))
			{
				vis.reset();
				sonar=true;
				e->marcar_para_borrar();
				e->recibir_visitante(vis);

				sumar_puntuacion(e->puntuacion_por_recoger());

				if(vis.es_recoge_ankh()) sistema_puntuacion.recoger_ankh();
				if(vis.es_recoge_salud()) jugador.recuperar_energia();
			}
		}

		if(sonar) cola_sonido(Recursos_audio::RS_DEFECTO, 128);
	}

	//Eliminar items recogidos...
	auto& items=mapa.acc_items();
	auto it_it=std::remove_if(items.begin(), items.end(), [](std::unique_ptr<Item_base>& p) {return p->es_para_borrar();});
	items.erase(it_it, items.end());

	//Eliminar proyectiles que han caducado.
	auto it_pr=std::remove_if(proyectiles.begin(), proyectiles.end(), [](std::unique_ptr<Proyectil>& p) {return p->es_para_borrar();});
	proyectiles.erase(it_pr, proyectiles.end());

	auto it_pre=std::remove_if(proyectiles_enemigos.begin(), proyectiles_enemigos.end(), [](std::unique_ptr<Proyectil>& p) {return p->es_para_borrar();});
	proyectiles_enemigos.erase(it_pre, proyectiles_enemigos.end());

	//Eliminar enemigos que sobran...
	auto it_ene=std::remove_if(enemigos.begin(), enemigos.end(), [](std::unique_ptr<Enemigo_base>& p) {return p->es_para_borrar();});
	if(it_ene != enemigos.end())
	{
		cola_sonido(Recursos_audio::RS_DESTRUIR_ENEMIGO, 92);
		enemigos.erase(it_ene, enemigos.end());
	}
}

void Controlador_juego::dibujar(DLibV::Pantalla& pantalla)
{
	pantalla.limpiar(0, 0, 0, 255);

	evaluar_enfoque_camara();

	dibujar_juego(pantalla);
	dibujar_hud(pantalla);

	if(fade.es_activo())
	{
		DLibV::Representacion_primitiva_caja_estatica caja_fade=DLibV::Representacion_primitiva_caja_estatica(
			DLibH::Herramientas_SDL::nuevo_sdl_rect(0, 0, 800, 600), fade.acc_r(), fade.acc_g(), fade.acc_b());

		caja_fade.establecer_alpha(fade.acc_alpha());
		caja_fade.volcar(pantalla);
	}
}

void Controlador_juego::dibujar_juego(DLibV::Pantalla& pantalla)
{
	std::vector<Representable *> vr;
	const Espaciable::t_caja caja(camara.acc_x(), camara.acc_y(), camara.acc_w(), camara.acc_h());

	mapa.acc_deco().obtener_celdas_representables(vr, caja);

	std::vector<Representable *> actores=mapa.obtener_actores_representables();
	vr.insert(vr.end(), actores.begin(), actores.end());

	for(auto& p : proyectiles) vr.push_back(p.get());
	for(auto& p : proyectiles_enemigos) vr.push_back(p.get());


	vr.push_back(&jugador);

	Ordenador_representables ordenador;
	std::sort(vr.begin(), vr.end(), ordenador);
	representador.generar_vista(pantalla, camara, vr);
/*
	SDL_Rect cp=DLibH::Herramientas_SDL::nuevo_sdl_rect(
		jugador.acc_espaciable_x()-camara.acc_x(),
		jugador.acc_espaciable_y()-camara.acc_y(),
		jugador.acc_espaciable_w(),
		jugador.acc_espaciable_h());

	DLibV::Representacion_primitiva_caja_estatica CAJA(cp, 255, 0, 0);
	CAJA.establecer_alpha(64);
	CAJA.volcar(pantalla);
*/
}

void Controlador_juego::dibujar_hud(DLibV::Pantalla& pantalla)
{
	std::vector<Rep_municion> municiones;
	unsigned int i=0,
		max_actual=control_armas.obtener_max_actual(),
		actual=control_armas.acc_municion_actual(),
		offset=control_armas.offset_hoja_sprites_arma_actual();

	while(i < control_armas.acc_max_arma())
	{
		unsigned int ver_offset=offset + (i >= max_actual);
		unsigned int alpha=i < actual ? 255 : 128;
		municiones.push_back(Rep_municion(i, alpha, ver_offset));
		++i;
	}

	//Polimorfizar..
	std::vector<Representable *> reps;
	for(auto& r : municiones) reps.push_back(&r);

	representador.generar_hud_municion(pantalla, reps);

	//Finalmente, al lado, poner la munición restante...
	std::string cadena_municion=to_string(control_armas.acc_reserva());
	DLibV::Representacion_texto_auto_estatica txt(pantalla.acc_renderer(), DLibV::Gestor_superficies::obtener(Recursos_graficos::RS_FUENTE_BASE), cadena_municion);
	txt.establecer_posicion(16, 18);
	txt.volcar(pantalla);

	//La vida...
	Frame_sprites fs_energia=Rep_municion::obtener_frame_hud(1);
	Frame_sprites fs_vidas=Rep_municion::obtener_frame_hud(2);
	representador.generar_hud_vida(pantalla, jugador.acc_energia(), sistema_vidas.acc_vidas(), fs_energia, fs_vidas);

	//Los ankh
	Frame_sprites fs_ankh=Rep_municion::obtener_frame_hud(3);
	representador.generar_hud_ankh(pantalla, sistema_puntuacion.acc_ankh_nivel(), mapa.acc_ankh_nivel(), sistema_puntuacion.acc_todos_ankh(), fs_ankh);

	//El nivel y tiempo...
	double dummy;
	double frac=modf(segundos_restantes,&dummy);
    	int decimas_restantes=round(frac*pow(10, 2)); //Dos decimales.
	representador.generar_hud_nivel(pantalla, mapa.acc_nombre(), segundos_restantes, decimas_restantes, sistema_puntuacion.acc_puntuacion());

}

void Controlador_juego::evaluar_enfoque_camara()
{
	int lim_nivel_w=mapa.acc_nivel().acc_w_en_celdas()*Celda::DIM_CELDA;
	int lim_nivel_h=mapa.acc_nivel().acc_h_en_celdas()*Celda::DIM_CELDA;

	if(lim_nivel_w <= camara.acc_w() && lim_nivel_h <= camara.acc_h())
	{
		int x=-(camara.acc_w()-lim_nivel_w) / 2;
		int y=-(camara.acc_h()-lim_nivel_h) / 2;
		camara.enfocar_a(x, y);
	}
	else
	{
		int x=jugador.acc_espaciable_x()-(camara.acc_w() / 2 );
		int y=jugador.acc_espaciable_y()-(camara.acc_h() / 2);
		camara.enfocar_a(x, y);
	}
}

//Cosas que pueden cambiar...

//TODO: Merge con turno nivel.

void Controlador_juego::logica_nivel()
{
	//Enemigos...

	class Vis:public Visitante_enemigo
	{
		private:
		const Jugador& jugador;

		public:
		Vis(const Jugador& j):
			Visitante_enemigo(),
			jugador(j) {}

		virtual void visitar(Enemigo_base&) {}
		virtual void visitar(Patrullador& e) {}
		virtual void visitar(Perseguidor& e) {e.recalcular_angulo(jugador);}
		virtual void visitar(Disparador& e) {e.recalcular_angulo(jugador);}
		virtual void visitar(Boss& e) {e.recalcular_angulo(jugador);}

	}vis(jugador);

	auto& enemigos=mapa.acc_enemigos();
	for(auto& e : enemigos) e->recibir_visitante(vis);
}

void Controlador_juego::turno_nivel(float delta)
{

	segundos_restantes-=delta;

	control_armas.turno(delta);

	//Proyectiles... Moverlos y comprobar si chocan con alguna celda. O si los del jugador se salen de la cámara.
	auto caja_camara=DLibH::Caja<float, unsigned int>{(float)camara.acc_x(), (float)camara.acc_y(), camara.acc_w(), camara.acc_h()};

	for(auto& p : proyectiles)
	{
		p->turno(delta);

		if(!p->copia_caja().es_en_colision_con(caja_camara)) p->marcar_para_borrar();
		else
		{
			auto c=mapa.acc_nivel().celdas_para_caja(p->copia_caja());
			if(std::any_of(c.begin(), c.end(), [](const Celda * c) {return c->es_solida();})) p->marcar_para_borrar();
		}

	}

	for(auto& p : proyectiles_enemigos)
	{
		p->turno(delta);
		auto c=mapa.acc_nivel().celdas_para_caja(p->copia_caja());
		if(std::any_of(c.begin(), c.end(), [](const Celda * c) {return c->es_solida();})) p->marcar_para_borrar();
	}

	//Enemigos...
	class Vis:public Visitante_enemigo
	{
		private:

		Mapa& mapa;
		float delta;
		std::vector<const Disparador *> enemigos_disparar;
		const Boss * boss_disparar;

		public:

		const std::vector<const Disparador *>& acc_enemigos_disparar() {return enemigos_disparar;}
		const Boss * acc_boss_disparar() {return boss_disparar;}

		Vis(Mapa& m,float d):
			Visitante_enemigo(), mapa(m), delta(d), boss_disparar(nullptr)
			{}

		virtual void visitar(Enemigo_base&) {}

		virtual void visitar(Disparador& e)
		{
			e.turno(delta);
			if(e.debe_disparar()) enemigos_disparar.push_back(&e);
		}

		virtual void visitar(Patrullador& e)
		{
			auto v=e.acc_vector() * Patrullador::VELOCIDAD;
			e.turno(delta);

			//TODO: Normalizar y aplicar al boss.

			if(v.x)
			{
				e.desplazar_caja(v.x * delta, 0.0);
				auto cx=mapa.acc_nivel().celdas_para_caja(e.copia_caja());
				if(std::any_of(cx.begin(), cx.end(), [](const Celda * c) {return c->es_solida();}))
				{
					auto col=cx[0];
					if(v.x > 0.0) e.ajustar_izquierda_de(*col);
					else e.ajustar_derecha_de(*col);
					e.girar();
					return;
				}
			}

			if(v.y)
			{
				e.desplazar_caja(0.0, v.y * delta);
				auto cy=mapa.acc_nivel().celdas_para_caja(e.copia_caja());
				if(std::any_of(cy.begin(), cy.end(), [](const Celda * c) {return c->es_solida();}))
				{
					auto col=cy[0];
					if(v.y > 0.0) e.ajustar_encima_de(*col);
					else e.ajustar_debajo_de(*col);
					e.girar();
					return;
				}
			}
		}

		virtual void visitar(Perseguidor& e)
		{
			auto v=e.acc_vector() * Perseguidor::VELOCIDAD;
			auto caja=e.copia_caja();

			e.turno(delta);

			e.desplazar_caja(v.x * delta, 0.0);

			auto cx=mapa.acc_nivel().celdas_para_caja(e.copia_caja());
			if(std::any_of(cx.begin(), cx.end(), [](const Celda * c) {return c->es_retiene_enemigo_tierra();}))
			{
				auto col=cx[0];
				if(v.x > 0.0) e.ajustar_izquierda_de(*col);
				else e.ajustar_derecha_de(*col);
			}

			e.desplazar_caja(0.0, v.y * delta);
			auto cy=mapa.acc_nivel().celdas_para_caja(e.copia_caja());
			if(std::any_of(cy.begin(), cy.end(), [](const Celda * c) {return c->es_retiene_enemigo_tierra();}))
			{
				auto col=cy[0];
				if(v.y > 0.0) e.ajustar_encima_de(*col);
				else e.ajustar_debajo_de(*col);

			}

			auto& enemigos=mapa.acc_enemigos();
			for(auto& enem : enemigos)
			{
				if( *(enem.get())!=e && enem->en_colision_con(e))
				{
					e.establecer_posicion(caja.origen.x, caja.origen.y);
					return;
				}
			}
		}

		virtual void visitar(Boss& e)
		{
			e.turno(delta);
			if(e.debe_disparar()) boss_disparar=&e;

			auto v=e.acc_vector() * Patrullador::VELOCIDAD;
			e.turno(delta);

			if(v.x)
			{
				e.desplazar_caja(v.x * delta, 0.0);
				auto cx=mapa.acc_nivel().celdas_para_caja(e.copia_caja());
				if(std::any_of(cx.begin(), cx.end(), [](const Celda * c) {return c->es_retiene_enemigo_tierra();}))
				{
					auto col=cx[0];
					if(v.x > 0.0) e.ajustar_izquierda_de(*col);
					else e.ajustar_derecha_de(*col);
					e.girar();
					return;
				}
			}

			if(v.y)
			{
				e.desplazar_caja(0.0, v.y * delta);
				auto cy=mapa.acc_nivel().celdas_para_caja(e.copia_caja());
				if(std::any_of(cy.begin(), cy.end(), [](const Celda * c) {return c->es_retiene_enemigo_tierra();}))
				{
					auto col=cy[0];
					if(v.y > 0.0) e.ajustar_encima_de(*col);
					else e.ajustar_debajo_de(*col);
					e.girar();
					return;
				}
			}
		}

	}vis(mapa, delta);

	auto& enemigos=mapa.acc_enemigos();
	for(auto& e : enemigos)
	{
		e->recibir_visitante(vis);

		if(e->es_disparable())
		{
			for(auto& p : proyectiles)
			{
				if(!p->es_para_borrar() && !e->es_para_borrar() && e->en_colision_con( * p ) )
				{
					p->marcar_para_borrar();
					e->restar_salud(p->acc_potencia());
					if(e->es_sin_salud())
					{
						e->marcar_para_borrar();
						sumar_puntuacion(e->puntuacion_por_eliminar());

						if(e->es_boss())
						{
							iniciar_fin_juego();
							return; //Salimos de aquí... Se reseteará todo
						}
					}
				}
			}
		}
	}

	auto& enemigos_disparar=vis.acc_enemigos_disparar();
	for(auto& e: enemigos_disparar) generar_disparo_enemigo(*e);

	if(vis.acc_boss_disparar()) generar_disparo_boss(*vis.acc_boss_disparar());
}

void Controlador_juego::sumar_puntuacion(unsigned int p)
{
	sistema_puntuacion.sumar_puntuacion(p);
	if(sistema_puntuacion.es_vida_extra())
	{
		sistema_vidas.sumar_vida();
		cola_sonido(Recursos_audio::RS_VIDA_EXTRA, 192);
		sistema_puntuacion.marcar_vida_como_reclamada();
	}
}

void Controlador_juego::generar_disparo_jugador()
{
	auto generar_angulo=[](unsigned int direccion, float rand)
	{
		DLibH::Vector_2d v;

		if(direccion & Espaciable::D_IZQUIERDA) v.x=-1.0;
		else if(direccion & Espaciable::D_DERECHA) v.x=1.0;

		if(direccion & Espaciable::D_ARRIBA) v.y=-1.0;
		else if(direccion & Espaciable::D_ABAJO) v.y=1.0;

		return DLibH::Vector_2d::obtener_angulo_para_vector_unidad_grados(v)+90.0+rand;
	};

	float x=jugador.acc_espaciable_x()+jugador.acc_espaciable_w()/2 - 2;	//-2 por el ancho y alto del proyectil.
	float y=jugador.acc_espaciable_y()+jugador.acc_espaciable_h()/2 - 2;
	Generador_int GPOT(-4, 4), GVOL(-20, 10);

	if(control_armas.acc_arma_actual()==Control_armas::t_armas::REVOLVER)
	{
		std::unique_ptr<Proyectil> p(new Proyectil(generar_angulo(jugador.acc_direccion(), 0.0), 250.0, 20+GPOT(), Proyectil::tfaccion::JUGADOR));
		p->establecer_posicion(x, y);
		proyectiles.push_back(std::move(p));
		cola_sonido(Recursos_audio::RS_REVOLVER, 128+GVOL());
	}
	else if(control_armas.acc_arma_actual()==Control_armas::t_armas::PISTOLA)
	{
		std::unique_ptr<Proyectil> p(new Proyectil(generar_angulo(jugador.acc_direccion(), 0.0), 300.0, 15+GPOT(), Proyectil::tfaccion::JUGADOR));
		p->establecer_posicion(x, y);
		proyectiles.push_back(std::move(p));
		cola_sonido(Recursos_audio::RS_PISTOLA, 128+GVOL());
	}
	else if(control_armas.acc_arma_actual()==Control_armas::t_armas::ESCOPETA)
	{
		Generador_int GT(3, 6), GI(-5, 5);
		unsigned int total=GT(), i=0;

		while(i < total)
		{
			std::unique_ptr<Proyectil> p(new Proyectil(generar_angulo(jugador.acc_direccion(), GI()), 220.0, 10+GPOT(), Proyectil::tfaccion::JUGADOR));
			p->establecer_posicion(x, y);
			proyectiles.push_back(std::move(p));
			++i;
		}

		cola_sonido(Recursos_audio::RS_ESCOPETA, 128+GVOL());
	}
}

void Controlador_juego::generar_disparo_enemigo(const Disparador& e)
{
	float x=e.acc_espaciable_x()+e.acc_espaciable_w()/2;
	float y=e.acc_espaciable_y()+e.acc_espaciable_h()/2;

	std::unique_ptr<Proyectil> p(new Proyectil(e.acc_angulo(), 100.0, 0, Proyectil::tfaccion::ENEMIGO));
	p->establecer_posicion(x, y);
	proyectiles_enemigos.push_back(std::move(p));
	cola_sonido(Recursos_audio::RS_DISPARO_ENEMIGO, 128);
}

void Controlador_juego::generar_disparo_boss(const Boss& e)
{
	float x=e.acc_espaciable_x()+e.acc_espaciable_w()/2;
	float y=e.acc_espaciable_y()+e.acc_espaciable_h()/2;

	std::vector<float> angulos;
	angulos.insert( angulos.end(), {-15.0, -10.0, -5.0, 0.0, 5.0, 10.0, 15.0});

	for(float &a : angulos)
	{
		std::unique_ptr<Proyectil> p(new Proyectil(e.acc_angulo_encarar()+a, 120.0, 0, Proyectil::tfaccion::ENEMIGO));
		p->establecer_posicion(x, y);
		proyectiles_enemigos.push_back(std::move(p));
	}

	cola_sonido(Recursos_audio::RS_DISPARO_ENEMIGO, 128);
}

void Controlador_juego::cola_sonido(unsigned int sonido, unsigned int vol)
{
	Audio::insertar_sonido(
		DLibA::Estructura_sonido(
		DLibA::Gestor_recursos_audio::obtener_sonido(sonido), vol));
}

void Controlador_juego::herir_jugador()
{
	jugador.herir();
	if(!jugador.acc_energia())
	{
		LOG<<"Insertando evento perder vida por herida"<<std::endl;
		cola_eventos.push(Evento_juego(Evento_juego::tipos::PERDER_VIDA_POR_ENEMIGO));
	}
	else
	{
		cola_sonido(Recursos_audio::RS_PERDER_ENERGIA, 192);
	}
}

void Controlador_juego::iniciar_fin_juego()
{
	if(sistema_puntuacion.acc_todos_ankh()==9) //No hay en el último nivel...
	{
		fade.activar(Fade::tcallback::FIN_1, 255, 255, 255);
	}
	else
	{
		fade.activar(Fade::tcallback::FIN_2, 128, 128, 128);
	}
}
