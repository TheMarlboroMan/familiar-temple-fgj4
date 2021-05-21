#include "jugador.h"
#include "env.h"

const float Jugador::SEGUNDOS_INVULNERABILIDAD=2.000;

Jugador::Jugador()
	:Actor(0, 0, 0, 0), Movil(),
	estado(new Estado_jugador_suelo()),
	TREC{env::make_data_path("data/recortes/jugador.dat")},
	direccion(Espaciable::D_DERECHA),
	energia(ENERGIA_INICIAL),
	tiempo_invulnerable(0.0)
{
	cambiar_estado(new Estado_jugador_suelo());
}

Jugador::~Jugador()
{

}

void Jugador::transformar_bloque(Bloque_transformacion_representable &b) const
{
	//TODO: Esta aproximación resultará del todo inútil cuando haya animación.
	class Vis:public Visitante_estado_jugador
	{
		private:

		const Input_usuario& input;

		public:

		unsigned int indice_sprite;

		Vis(const Input_usuario& i):input(i), indice_sprite(0) {}

		virtual void visitar(Estado_jugador&) {}
		virtual void visitar(Estado_jugador_suelo&) {indice_sprite=0;}
		virtual void visitar(Estado_jugador_aire&) {indice_sprite=10;}
		virtual void visitar(Estado_jugador_recargar&) {indice_sprite=20;}
	}vis(input);
	//Se asume que todos los frames van mirando a la derecha.

	b.establecer_tipo(Bloque_transformacion_representable::TR_BITMAP);
	b.establecer_alpha(tiempo_invulnerable ? 128 : 255);
	b.establecer_recurso(Recursos_graficos::RT_SPRITES);

	auto generar_angulo=[](unsigned int direccion)
	{
		DLibH::Vector_2d v;
		if(direccion & Espaciable::D_IZQUIERDA) v.x=-1.0;
		else if(direccion & Espaciable::D_DERECHA) v.x=1.0;
		if(direccion & Espaciable::D_ARRIBA) v.y=-1.0;
		else if(direccion & Espaciable::D_ABAJO) v.y=1.0;
		return DLibH::Vector_2d::obtener_angulo_para_vector_unidad_grados(v);
	};
	b.rotar(generar_angulo(direccion));

	estado->recibir_visitante(vis);
	Frame_sprites f=TREC.obtener(vis.indice_sprite);

	if(f)
	{
		b.establecer_recorte(f.x, f.y, f.w, f.h);

	/*Cuando el ancho de la caja lógica sea distinto del ancho de la caja
	de representación ajustaremos la posición.*/

		if(f.w != acc_espaciable_w())
		{
			f.desp_x-=f.w-acc_espaciable_w();
		}

		b.establecer_posicion(acc_espaciable_x()+f.desp_x+(info_salto.z/2), acc_espaciable_y()+f.desp_y+(info_salto.z/2), f.w+(-info_salto.z), f.h+(-info_salto.z));
	}
}

void Jugador::procesar_estado(float delta)
{
	//Cargar input...
	estado->cargar_datos(acc_vector(), input);

	//Se realizaría el proceso por parte del estado.
	estado->procesar(delta);

	//Actualizar los datos...
	auto v=estado->acc_vector();
	establecer_vector(v.x, Movil::V_X);
	establecer_vector(v.y, Movil::V_Y);
}

void Jugador::saltar()
{
	cambiar_estado(new Estado_jugador_aire());
	info_salto.vz=-70.0;
}

void Jugador::turno(float delta)
{
	if(es_en_aire())
	{
		//Integrar gravedad.
		integrar_vector(delta, info_salto.vz, 150.0);	//100.0 es el peso.

		//Calcular nueva z.
		info_salto.z+=delta * info_salto.vz;

		if(info_salto.z >= 0.0)
		{
			cambiar_estado(new Estado_jugador_suelo());
			info_salto.reset();
		}
	}

	//Realizar el turno del estado, que puede contemplar
	//un cambio a otro estado.


	estado->turno(delta);
	if(estado->es_cambia_estado())
	{
		Estado_jugador::t_estados ne=estado->acc_nuevo_estado();

		switch(ne)
		{
//			case Estado_jugador::t_estados::AIRE: cambiar_estado(new Estado_jugador_aire()); break;
			case Estado_jugador::t_estados::SUELO: cambiar_estado(new Estado_jugador_suelo()); break;
//			case Estado_jugador::t_estados::RECARGAR: cambiar_estado(new Estado_jugador_recargar()); break;
			default:
				LOG<<"[WARNING]: Se recibe estado en turno jugador que no está controlado."<<std::endl;
			break;
		}
	}

	if(tiempo_invulnerable)
	{
		tiempo_invulnerable-=delta;
		if(tiempo_invulnerable < 0.0) tiempo_invulnerable=0.0;
	}
}

void Jugador::cambiar_estado(Estado_jugador * e)
{
	Estado_jugador::t_caja caja=Estado_jugador::calcular_cambio_estados(*(estado.get()), *e);
	estado.reset(e);
	mut_w_caja(caja.w);
	mut_h_caja(caja.h);
	desplazar_caja(caja.origen.x, caja.origen.y);
}

void Jugador::chocar_desde_izquierda_con(const Espaciable::t_caja& c)
{
	establecer_vector(0.0, Movil::V_X);
	ajustar_izquierda_de(c);
}

void Jugador::chocar_desde_derecha_con(const Espaciable::t_caja& c)
{
	establecer_vector(0.0, Movil::V_X);
	ajustar_derecha_de(c);
}

void Jugador::chocar_desde_arriba_con(const Espaciable::t_caja& c)
{
	establecer_vector(0.0, Movil::V_Y);
	ajustar_encima_de(c);
}

void Jugador::chocar_desde_abajo_con(const Espaciable::t_caja& c)
{
	establecer_vector(0.0, Movil::V_Y);
	ajustar_debajo_de(c);
}
