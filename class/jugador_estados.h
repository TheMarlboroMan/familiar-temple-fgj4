#ifndef JUGADOR_ESTADOS_H
#define JUGADOR_ESTADOS_H

#include <memory>
#include <iostream>

#include "input_usuario.h"
#include "../libdan2/herramientas/vector_2d/vector_2d.h"
#include "../libdan2/herramientas/caja/caja.h"

class Jugador;

class Estado_jugador;
class Estado_jugador_suelo;
class Estado_jugador_aire;
class Estado_jugador_recargar;

class Visitante_estado_jugador
{
	public:

	Visitante_estado_jugador() {}

//	virtual void visitar(Estado_jugador&)=0;
	virtual void visitar(Estado_jugador_suelo&)=0;
	virtual void visitar(Estado_jugador_aire&)=0;
	virtual void visitar(Estado_jugador_recargar&)=0;
};


class Estado_jugador
{
	//////////////////
	// Definiciones.
	public:

	enum class t_estados {
		SUELO, 				//Estado normal en suelo.
		AIRE, 				//Estado cayendo o saltando.
		RECARGAR

	};

	typedef DLibH::Caja<int, unsigned int> t_caja;	//La caja que usaremos para definir transformaciones entre estados.

	//Todas las medidas en unidades por segundo, salvo que se diga lo contrario.
	static const float VEL_MINIMA_SUELO;
	static const float VEL_MAXIMA_SUELO;
	static const float VELOCIDAD_FRENO_MANUAL;
	static const float ACELERACION_SUELO;
	static const float DECELERACION_SUELO;

	////////////////////
	//Propiedades...

	private:

	DLibH::Vector_2d v;
	Input_usuario iu;
	bool cambia_estado;
	t_estados nuevo_estado;

	////////////////////
	//Métodos

	protected:

	void reducir_valor_con_delta(float &v, float delta)
	{
		v-=delta;
		if(v < 0.0) v=0.0;
	}

	////////////////////
	//Interface.

	public:

	/*Este devolvería la caja que se aplica para transformar un estado en
	otro. Todos los estados se comparan con el método "caja_original" que
	se define aquí, que se basaría en "estar en pie.*/

	virtual t_caja obtener_caja_estado() const=0;
	virtual t_estados acc_estado() const=0;
	virtual void recibir_visitante(Visitante_estado_jugador&)=0;

	/*El método procesar debe alterar el vector y la dirección en función
	del input del jugador, si tal cosa es posible.*/
	virtual void procesar(float delta)=0;
	virtual void turno(float delta)=0;

	//////////////////
	//Implementar.

	static t_caja calcular_cambio_estados(const Estado_jugador& anterior, const Estado_jugador& nuevo)
	{
		//Estado base...
		t_caja resultado(0, 0, 16, 40);

		//Salida del estado anterior... Restaremos la y y la x al
		//estado base.
		t_caja caja_anterior=anterior.obtener_caja_estado();
		resultado.origen.x-=caja_anterior.origen.x;
		resultado.origen.y-=caja_anterior.origen.y;

		//Entrada del estado nuevo... Sumando los valores del nuevo.
		t_caja caja_nuevo=nuevo.obtener_caja_estado();
		resultado.origen.x+=caja_nuevo.origen.x;
		resultado.origen.y+=caja_nuevo.origen.y;
		resultado.w=caja_nuevo.w;
		resultado.h=caja_nuevo.h;

		return resultado;
	}

	void cargar_datos(const DLibH::Vector_2d pv, const Input_usuario& piu)
	{
		v=pv;
		iu=piu;
	}

	const DLibH::Vector_2d& acc_vector() const {return v;}
	const Input_usuario& acc_input() const {return iu;}
	bool es_cambia_estado() const {return cambia_estado;}
	const t_estados acc_nuevo_estado() const {return nuevo_estado;}

	void establecer_vector_x(float x) {v.x=x;}
	void establecer_vector_y(float y) {v.y=y;}
	void cambiar_estado(t_estados e)
	{
		cambia_estado=true;
		nuevo_estado=e;
	}

	Estado_jugador()
		:cambia_estado(false), nuevo_estado(t_estados::SUELO)
	{}
	virtual ~Estado_jugador() {}
};

/******************************************************************************/

class Estado_jugador_suelo:public Estado_jugador
{
	private:

	float margen_recarga;	//Si pulsamos dos veces seguidas el botón de mantener la posición recargaremos.
	bool recargar;

	public:

	Estado_jugador_suelo()
		:margen_recarga(0.0), recargar(false)
	{}

	bool es_recargar() const {return recargar;}

	virtual t_caja obtener_caja_estado() const {return t_caja(0, 0, 16, 16);}
	virtual t_estados acc_estado() const {return t_estados::SUELO;}
	virtual void recibir_visitante(Visitante_estado_jugador& v) {v.visitar(*this);}

	virtual void procesar(float delta)
	{
		//TODO: Mover este auto a algún tipo de método externo para esta clase o namespace..

		auto f=[delta](int mov, float vreal, float v)
		{
			if(!mov && vreal)
			{
				v-=DECELERACION_SUELO * delta;
				if(v < 0.0) v=0.0;
				else if(v > VEL_MAXIMA_SUELO) v=VEL_MAXIMA_SUELO; //En efecto, para controlar la salida del salto largo.
				v*= vreal > 0.0 ? 1.0 : -1.0;
			}
			else if(mov)
			{
				bool misma_direccion=(vreal >= 0.0 && mov > 0) || (vreal <= 0.0 && mov < 0);

				//En la misma dirección aceleraremos.
				if(misma_direccion)
				{
					v+=delta * ACELERACION_SUELO;
					if(v < VEL_MINIMA_SUELO) v=VEL_MINIMA_SUELO;
					else if(v > VEL_MAXIMA_SUELO) v=VEL_MAXIMA_SUELO;
					v*=(float)mov;
				}
				//En la dirección contraria frenaremos. Si llegamos a la velocidad mínima invertimos el sentido de la marcha.
				else
				{
					v-=delta * VELOCIDAD_FRENO_MANUAL;
					if(v < VEL_MINIMA_SUELO) //Ahora es posible dar la vuelta, de modo que ponemos el vector apuntando hacia la dirección deseada.
					{
						v=VEL_MINIMA_SUELO;
						v*=(float)mov;
					}
					else //Aún no es posible dar la vuelta. Ponemos al vector apuntando hacia el vector actual.
					{
						v*=vreal < 0.0 ? -1.0 : 1.0;
					}
				}
			}

			return v;
		};

		auto input=acc_input();
		establecer_vector_x( f(input.mov_horizontal, acc_vector().x, fabs(acc_vector().x)) );
		establecer_vector_y( f(input.mov_vertical, acc_vector().y, fabs(acc_vector().y)) );

		//Si esto ocurre es porque hemos intentado recargar pero el
		//controlador no nos deja. Reiniaríamos el estado.
		if(recargar)
		{
			recargar=false;
			margen_recarga=0.5;
		}

		//Doble keydown en fijar para recargar...
		if(input.keydown_apuntar)
		{
			if(!margen_recarga) margen_recarga=0.5;
			else recargar=true;
		}

		if(input.recargar) {

			recargar=true;
		}
	}

	virtual void turno(float delta)
	{
		reducir_valor_con_delta(margen_recarga, delta);
	}
};

//TODO: Es copia del que está arriba...

class Estado_jugador_recargar:public Estado_jugador
{
	private:

	float tiempo_recarga;

	public:

	//TODO: Different weapons, different times.
	Estado_jugador_recargar(float _t): tiempo_recarga(_t)
	{}

	virtual t_caja obtener_caja_estado() const {return t_caja(0, 0, 16, 16);}
	virtual t_estados acc_estado() const {return t_estados::RECARGAR;}
	virtual void recibir_visitante(Visitante_estado_jugador& v) {v.visitar(*this);}

	virtual void procesar(float delta)
	{
		auto f=[delta](int mov, float vreal, float v)
		{
			if(!mov && vreal)
			{
				v-=DECELERACION_SUELO * delta;
				if(v < 0.0) v=0.0;
				else if(v > VEL_MAXIMA_SUELO) v=VEL_MAXIMA_SUELO; //En efecto, para controlar la salida del salto largo.
				v*= vreal > 0.0 ? 1.0 : -1.0;
			}
			else if(mov)
			{
				bool misma_direccion=(vreal >= 0.0 && mov > 0) || (vreal <= 0.0 && mov < 0);

				//En la misma dirección aceleraremos.
				if(misma_direccion)
				{
					v+=delta * ACELERACION_SUELO;
					if(v < VEL_MINIMA_SUELO) v=VEL_MINIMA_SUELO;
					else if(v > VEL_MAXIMA_SUELO) v=VEL_MAXIMA_SUELO;
					v*=(float)mov;
				}
				//En la dirección contraria frenaremos. Si llegamos a la velocidad mínima invertimos el sentido de la marcha.
				else
				{
					v-=delta * VELOCIDAD_FRENO_MANUAL;
					if(v < VEL_MINIMA_SUELO) //Ahora es posible dar la vuelta, de modo que ponemos el vector apuntando hacia la dirección deseada.
					{
						v=VEL_MINIMA_SUELO;
						v*=(float)mov;
					}
					else //Aún no es posible dar la vuelta. Ponemos al vector apuntando hacia el vector actual.
					{
						v*=vreal < 0.0 ? -1.0 : 1.0;
					}
				}
			}

			return v;
		};

		auto input=acc_input();
		establecer_vector_x( f(input.mov_horizontal, acc_vector().x, fabs(acc_vector().x)) );
		establecer_vector_y( f(input.mov_vertical, acc_vector().y, fabs(acc_vector().y)) );
	}

	virtual void turno(float delta)
	{
		reducir_valor_con_delta(tiempo_recarga, delta);
		if(!tiempo_recarga) cambiar_estado(t_estados::SUELO);
	}
};

class Estado_jugador_aire:public Estado_jugador
{
	public:

	virtual t_caja obtener_caja_estado() const {return t_caja(0, 0, 16, 16);}
	virtual t_estados acc_estado() const {return t_estados::AIRE;}
	virtual void recibir_visitante(Visitante_estado_jugador& v) {v.visitar(*this);}

	virtual void procesar(float delta) {}
	virtual void turno(float delta) {}
};

#endif
