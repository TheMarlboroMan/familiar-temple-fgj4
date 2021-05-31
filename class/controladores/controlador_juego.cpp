#include "controlador_juego.h"
#include <cassert>
#include "../env.h"

#ifdef WINCOMPIL
	using namespace parche_mingw;
#else
	using namespace std;
#endif

Controlador_juego::Controlador_juego(
	const DLibH::Controlador_argumentos& CARG, 
	hi_score_manager& _hi_score, 
	Sistema_estados& s
)
	:Controlador_interface(s),
	hi_scores{_hi_score},
	cargador_mapas(env::make_data_path("data/recursos/niveles.txt")),
	nivel_actual(1), segundos_restantes(0.0), level_time{0.f}, game_time{0.f}
{
	camara=DLibV::Camara(0, 0, 800, 600);

	int pos_nivel=CARG.buscar("-n");

	if(pos_nivel!=-1)
	{
		if(pos_nivel != -1) nivel_actual=std::atoi(CARG.acc_argumento(pos_nivel+1).c_str());
	}
	
	int index_god_mode=CARG.buscar("-god");
	if(index_god_mode!=-1) {
		
		LOG<<"god mode enabled"<<std::endl;
		god_mode=true;
	}

	importar_nivel(nivel_actual);
}

bool Controlador_juego::loop(const Input& input, float delta)
{
	//DLibA::Controlador_audio_SDL::obtener()->debug_estado_canales();

	if( (input.es_senal_salida() || input.es_input_down(Input::I_ESCAPE)) && !fade.es_activo())
	{
		fade.activar(Fade::tcallback::VOLVER_TITULO, 0, 0, 0);
	}
	else
	{
		if(!fade.es_activo())
		{
			Input_usuario iu=recoger_input_usuario(input);
			if(paused) {
			
				return true;
			}
		
			logica_nivel();

			float delta_world=delta,
				  delta_player=delta;

			if(focus_control.is_active() && focus_control.get_current()==powers::power_type::time) {

				delta_world*=0.4;
				delta_player*=0.75;
			}

			game_time+=delta;
			turno_nivel(delta_world);
			procesar_input_jugador(iu, delta_player);
			logica_jugador(delta_player);
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
			hi_scores.submit(hi_score{nivel_actual, sistema_puntuacion.acc_puntuacion(), false, game_time});
			reset();
			solicitar_cambio_estado(Sistema_estados::estados::E_GAME_OVER);
		break;

		case Fade::tcallback::FIN_1:
			hi_scores.submit(hi_score{nivel_actual, sistema_puntuacion.acc_puntuacion(), true, game_time});
			reset();
			solicitar_cambio_estado(Sistema_estados::estados::E_FIN_1);
		break;

		case Fade::tcallback::FIN_2:
			hi_scores.submit(hi_score{nivel_actual, sistema_puntuacion.acc_puntuacion(), true, game_time});
			reset();
			solicitar_cambio_estado(Sistema_estados::estados::E_FIN_2);
		break;
	}

	fade.finalizar();
}

void Controlador_juego::procesar_input_jugador(const Input_usuario& iu, float delta)
{
	
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
		const Input_usuario& iu;

		bool intentar_disparar{false},
			intentar_disparar_auto{false},
			recargar{false},
			saltar{false},
			focus{false},
			discard{false};

		public:

		bool es_intentar_disparar() const {return intentar_disparar;}
		bool es_intentar_disparar_auto() const {return intentar_disparar_auto;}
		bool es_recargar() const {return recargar;}
		bool es_saltar() const {return saltar;}
		bool is_focus() const {return focus;}
		bool is_discard() const {return discard;}

		Vis(Jugador& pj, const Input_usuario& pi): //, Nivel& pn):
			jugador(pj), iu(pi)
		{}

		virtual void visitar(Estado_jugador_suelo& e)
		{
			if(iu.focus) {
				focus=true;
			}

			if(e.es_recargar()) recargar=true;
			else if(iu.discard_weapon) discard=true;
			else if(iu.saltar && jugador.puede_saltar()) saltar=true;
			else if(iu.disparar) intentar_disparar=true;
			else if(iu.disparar_auto) intentar_disparar_auto=true;
		}

		virtual void visitar(Estado_jugador_recargar&) {}
		virtual void visitar(Estado_jugador_aire&) {}

	}vis(jugador, iu);

	jugador.recibir_visitante_estado(vis);

	//Finalmente, según el estado final, procesar el input.
#ifndef NDEBUG
	Estado_jugador::t_estados copia_estado=jugador.obtener_estado();
#endif

	jugador.procesar_estado(delta);

#ifndef NDEBUG
	assert(jugador.obtener_estado()==copia_estado);
#endif

	if(vis.is_focus() && focus_control.can_be_activated()) {

		activate_focus();
	}

	if(vis.is_discard() && control_armas.can_discard_current()) {

		control_armas.discard_current();
		cola_sonido(Recursos_audio::RS_RECARGAR, 128);
	}

	if(vis.es_intentar_disparar() || vis.es_intentar_disparar_auto()) {


		if(control_armas.is_empty()) {

			if(vis.es_intentar_disparar()) {

				cola_sonido(Recursos_audio::RS_ARMA_VACIA, 128);
			}
			//machine gun hack.
			else if(vis.es_intentar_disparar_auto() && !control_armas.has_cooled_off()) {

				cola_sonido(Recursos_audio::RS_ARMA_VACIA, 128);
			}
		}
		else {

			bool fire{false};

			if(vis.es_intentar_disparar())
			{
				fire=control_armas.puede_disparar();
			}
			else if(vis.es_intentar_disparar_auto()) {

				fire=control_armas.puede_disparar_auto();
			}

			if(fire) {

				generar_disparo_jugador();
				control_armas.disparar();
			}
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
			jugador.recargar(control_armas.get_reload_time());
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

		level_time=mapa.acc_segundos_finalizar();
		segundos_restantes=level_time;

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
	focus_control.reset();
	game_time=0.f;

	proyectiles.clear();
	proyectiles_enemigos.clear();

	importar_nivel(nivel_actual);
}

void Controlador_juego::reiniciar_nivel()
{
	LOG<<"RESTANDO VIDA"<<std::endl;
	sistema_vidas.restar_vida();
	focus_control.force_stop();
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

	if(input.es_input_down(Input::I_PAUSE)) {
	
		paused=!paused;		
	}

	if(paused) {
		
		return iu;
	}

	if(input.es_input_pulsado(Input::I_DERECHA)) iu.mov_horizontal=1;
	else if(input.es_input_pulsado(Input::I_IZQUIERDA)) iu.mov_horizontal=-1;
	else if(input.es_input_up(Input::I_DERECHA)) iu.keyup_horizontal=1;
	else if(input.es_input_up(Input::I_IZQUIERDA)) iu.keyup_horizontal=-1;

	if(input.es_input_pulsado(Input::I_ARRIBA)) iu.mov_vertical=-1;
	else if(input.es_input_pulsado(Input::I_ABAJO)) iu.mov_vertical=1;

	if(input.es_input_down(Input::I_SALTAR)) iu.saltar=true;
	if(input.es_input_down(Input::I_DISPARAR)) iu.disparar=true;
	if(input.es_input_pulsado(Input::I_DISPARAR)) iu.disparar_auto=true;

	if(input.es_input_pulsado(Input::I_APUNTAR)) iu.apuntar=true;
	if(input.es_input_down(Input::I_APUNTAR)) iu.keydown_apuntar=true;

	if(input.es_input_down(Input::I_RECARGAR)) iu.recargar=true;
	if(input.es_input_down(Input::I_FOCUS)) iu.focus=true;
	if(input.es_input_down(Input::I_DISCARD_WEAPON)) iu.discard_weapon=true;

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

	focus_control.step(delta);
	control_armas.turno(delta);
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
			
				if(sistema_puntuacion.acc_ankh_nivel()!=mapa.acc_ankh_nivel())
				{
					LOG<<"Failing ANKH in level"<<nivel_actual<<" got "<<sistema_puntuacion.acc_ankh_nivel()<<" vs "<<mapa.acc_ankh_nivel()<<std::endl;
					sistema_puntuacion.fail_all_ankh();	
				}
				else {
					LOG<<"Finalizado nivel "<<nivel_actual<<" con todos ANKH"<<std::endl;
				}
				
				//Add 10 points per extra second.
				sumar_puntuacion(floor(segundos_restantes)*10);
				
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
					if(!god_mode) {
				
						LOG<<"Insertando evento perder vida por celda"<<std::endl;
						cola_eventos.push(Evento_juego(Evento_juego::tipos::PERDER_VIDA_POR_CELDA));
						return;
					}
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
		virtual void visitar(Item_subfusil& i) {recoger_arma(Control_armas::t_armas::SUBFUSIL, 60);}
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
				add_focus(2);

				if(vis.es_recoge_ankh()) {
					sistema_puntuacion.recoger_ankh();
					//TODO: Erase this.
					sumar_puntuacion(200);
					add_focus(6); //extra focus 
				}

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
}

void Controlador_juego::dibujar_hud(
	DLibV::Pantalla& _screen
)
{
	bool show_level_name=segundos_restantes >= (level_time - 4.);

	//representador.reload_data();

	//Draw overlay boxes
	representador.hud_overlay(_screen, show_level_name);
	
	//Ammo
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
	for(auto& r : municiones) {
		reps.push_back(&r);
	}

	representador.hud_ammo(_screen, reps, control_armas.acc_reserva());
	
	int index=0;
	switch(focus_control.get_current()) {
	
		case powers::power_type::time: index=70; break;
		case powers::power_type::fire: index=71; break;
		case powers::power_type::health: index=72; break;
		case powers::power_type::ammo: index=73; break;
		
	}
	
	Frame_sprites fs_current_power=Rep_municion::obtener_frame_hud(index);	
	representador.hud_focus(_screen, focus_control.get_focus(), fs_current_power, focus_control.will_switch_shortly());
	
	auto fs_full_health=Rep_municion::obtener_frame_hud(1),
		fs_empty_health=Rep_municion::obtener_frame_hud(4);
		
	representador.hud_health(_screen, jugador.acc_energia(), fs_full_health, fs_empty_health);
	representador.hud_timer(_screen, segundos_restantes, segundos_restantes < (level_time / 3.f) );
	
	Frame_sprites fs_ankh=Rep_municion::obtener_frame_hud(3);
	representador.hud_ankh(_screen, sistema_puntuacion.acc_ankh_nivel(), mapa.acc_ankh_nivel(), fs_ankh);
	representador.hud_score(_screen, sistema_puntuacion.acc_puntuacion());
	
	
	if(show_level_name) {
	
		representador.hud_level_name(_screen, mapa.acc_nombre(), nivel_actual, sistema_vidas.acc_vidas(), Rep_municion::obtener_frame_hud(2));
	}
	
	representador.power_overlay(_screen, focus_control.is_active(), focus_control.get_current(), Rep_municion::obtener_frame_hud(50), Rep_municion::obtener_frame_hud(60));
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
					
					add_focus(1); //focus per hit 
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

void Controlador_juego::add_focus(int _quantity) {

	focus_control.add_focus(_quantity);
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

	
	int multiplicador=focus_control.is_active() && focus_control.get_current()==powers::power_type::fire
		? 2
		: 1;
	Generador_int GPOT(-4, 4), GVOL(-20, 10);

	switch(control_armas.acc_arma_actual()) {

		case Control_armas::t_armas::REVOLVER:{
			std::unique_ptr<Proyectil> p(new Proyectil(generar_angulo(jugador.acc_direccion(), 0.0), 250.0, 21+GPOT(), Proyectil::tfaccion::JUGADOR, multiplicador));
			p->establecer_posicion(x, y);
			proyectiles.push_back(std::move(p));
			cola_sonido(Recursos_audio::RS_REVOLVER, 128+GVOL());
		}
		break;
		case Control_armas::t_armas::PISTOLA: {
		    Generador_int GI(-2, 2);
			std::unique_ptr<Proyectil> p(new Proyectil(generar_angulo(jugador.acc_direccion(), GI()), 300.0, 15+GPOT(), Proyectil::tfaccion::JUGADOR, multiplicador));
			p->establecer_posicion(x, y);
			proyectiles.push_back(std::move(p));
			cola_sonido(Recursos_audio::RS_PISTOLA, 128+GVOL());
		}
		break;
		case Control_armas::t_armas::ESCOPETA: {
			Generador_int GT(4, 9), GI(-8, 8);
			unsigned int total=GT(), i=0;

			while(i < total)
			{
				std::unique_ptr<Proyectil> p(new Proyectil(generar_angulo(jugador.acc_direccion(), GI()), 220.0, 10+GPOT(), Proyectil::tfaccion::JUGADOR, multiplicador));
				p->establecer_posicion(x, y);
				proyectiles.push_back(std::move(p));
				++i;
			}

			cola_sonido(Recursos_audio::RS_ESCOPETA, 128+GVOL());
		}
		break;
		case Control_armas::t_armas::SUBFUSIL: {
			Generador_int GI(-4, 4);
			std::unique_ptr<Proyectil> p(new Proyectil(generar_angulo(jugador.acc_direccion(), GI()), 280.0, 8+GPOT(), Proyectil::tfaccion::JUGADOR, multiplicador));
			p->establecer_posicion(x, y);
			proyectiles.push_back(std::move(p));

			cola_sonido(Recursos_audio::RS_PISTOLA, 80+GVOL());
		}
		break;
		default:
			throw std::runtime_error("unregistered weapon in generar_disparo_jugador");

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
	if(god_mode) {
	
		return;
	}

	jugador.herir();
	if(!jugador.acc_energia())
	{
		LOG<<"Insertando evento perder vida por herida"<<std::endl;
		cola_eventos.push(Evento_juego(Evento_juego::tipos::PERDER_VIDA_POR_ENEMIGO));
	}
	else
	{
		add_focus(3);
		cola_sonido(Recursos_audio::RS_PERDER_ENERGIA, 192);
	}
}

void Controlador_juego::iniciar_fin_juego()
{	
	if(sistema_puntuacion.is_all_ankh()) {
	
		fade.activar(Fade::tcallback::FIN_1, 255, 255, 255);
	}
	else {
		fade.activar(Fade::tcallback::FIN_2, 128, 128, 128);
	}
}

void Controlador_juego::activate_focus() {

	focus_control.activate();
	//automatically refill weapon
	control_armas.rellenar();
	cola_sonido(Recursos_audio::RS_PERDER_VIDA, 128);

	switch(focus_control.get_current()) {

		case powers::power_type::ammo: {

			int ammo_to_add=0;
			switch(control_armas.acc_arma_actual()) {

				case Control_armas::t_armas::PISTOLA: ammo_to_add=24; break;
				case Control_armas::t_armas::REVOLVER: ammo_to_add=12; break;
				case Control_armas::t_armas::ESCOPETA: ammo_to_add=8; break;
				case Control_armas::t_armas::SUBFUSIL: ammo_to_add=60; break;
				case Control_armas::t_armas::NONE: 
					throw std::runtime_error("This cannot happen");
				break;
			}

			control_armas.sumar_reserva(
				control_armas.acc_arma_actual(),
				ammo_to_add
			);
		}
		break;
		case powers::power_type::health:
			jugador.recuperar_energia();
		break;
		case powers::power_type::fire:
		case powers::power_type::time:
		break;
	}
}
