#ifndef CONTROL_ARMAS_H
#define CONTROL_ARMAS_H

#include <map>
#include "../graficos/representable.h"
#include "../herramientas_proyecto/tabla_sprites.h"
#include "../recursos/recursos.h"

class Control_armas
{
	//////////////
	//Definiciones
	public:

	enum class t_armas {REVOLVER=10, ESCOPETA=20, PISTOLA=30};
	static const int MAX_ARMAS=3;

	class Info_municion
	{
		private:

		unsigned int max_arma;
		unsigned int municion_actual;
		unsigned int reserva;
		float cooloff;	
		float val_cooloff;

		public:

		Info_municion():max_arma(0), municion_actual(0), reserva(0), cooloff(0.0), val_cooloff(0.0)
		{}		

		Info_municion(unsigned int max, unsigned int act, unsigned int res, float vc):max_arma(max), municion_actual(act), reserva(res), cooloff(0.0), val_cooloff(vc)
		{}		

		void reset(unsigned int max, unsigned int act, unsigned int res)
		{
			max_arma=max;
			municion_actual=act;
			reserva=res;
			cooloff=0.0;
		}

		bool puede_disparar() const {return municion_actual && !cooloff;}
		bool es_arma_llena() const {return municion_actual==obtener_max_actual();}
		void disparar() 
		{
			--municion_actual;
			cooloff=val_cooloff;
		}
		void recargar() 
		{
			unsigned int vacias=max_arma - municion_actual;

			if(reserva)
			{
				if(reserva >= vacias) 
				{
					reserva-=vacias;
					municion_actual=obtener_max_actual();
				}
				else
				{
					municion_actual=reserva;
					reserva=0;
				}
			}
			else
			{
				municion_actual=obtener_max_actual();
			}
		}

		void rellenar() {municion_actual=max_arma;}

		unsigned int acc_reserva() const {return reserva;}
		unsigned int acc_max_arma() const {return max_arma;}
		unsigned int obtener_max_actual() const {if(reserva) return max_arma; else return max_arma / 2;}
		unsigned int acc_municion_actual() const {return municion_actual;}

		void sumar_reserva(unsigned int v) {reserva+=v;}

		void turno(float delta)
		{
			if(cooloff)
			{
				cooloff-=delta;
				if(cooloff < 0.0) cooloff=0.0;
			}
		}
	};

	///////////
	//Propiedades
	private:

	std::map<t_armas, Info_municion> info_armas;
	t_armas arma_actual;

	////////////
	//Métodos internos.
	private:

	void iniciar();

	///////////
	// Interface pública...	
	public:

	Control_armas();
	~Control_armas();

	void reset();

	t_armas acc_arma_actual() {return arma_actual;}
	void mut_arma_actual(t_armas arma) {arma_actual=arma;}

		//Proxies.
	void sumar_reserva(t_armas arma, unsigned int cantidad) {info_armas[arma].sumar_reserva(cantidad);}

	void disparar() {info_armas[arma_actual].disparar();}
	unsigned int acc_reserva() {return info_armas[arma_actual].acc_reserva();}
	unsigned int acc_municion_actual() {return info_armas[arma_actual].acc_municion_actual();}
	unsigned int acc_max_arma() {return info_armas[arma_actual].acc_max_arma();}
	unsigned int obtener_max_actual() {return info_armas[arma_actual].obtener_max_actual();}
	bool puede_disparar() {return info_armas[arma_actual].puede_disparar();}
	bool es_arma_llena() {return info_armas[arma_actual].es_arma_llena();}
	void recargar() {info_armas[arma_actual].recargar();}
	void rellenar() {info_armas[arma_actual].rellenar();}
	void turno(float delta) {info_armas[arma_actual].turno(delta);}
	unsigned int offset_hoja_sprites_arma_actual() const
	{
		switch(arma_actual)
		{
			case t_armas::REVOLVER: return 10; break;
			case t_armas::ESCOPETA: return 20; break;
			case t_armas::PISTOLA: return 30; break;
			default: return 0; break;
		}
	}

//	unsigned int acc_municion_actual() const {return info_armas[municion_actual].municion_actual();}
};

class Rep_municion:public Representable
{
	private:

	unsigned int x;
	unsigned int alpha;
	unsigned int offset;
	static Tabla_sprites TREC;

	public:
	virtual unsigned short int obtener_profundidad_ordenacion() const {return 999;}
	virtual void transformar_bloque(Bloque_transformacion_representable &b) const
	{
		b.establecer_tipo(Bloque_transformacion_representable::TR_BITMAP);
		b.establecer_alpha(alpha);
		b.establecer_recurso(Recursos_graficos::RT_HUD);

		Frame_sprites f=TREC.obtener(offset);
		if(f)
		{
			b.establecer_recorte(f.x, f.y, f.w, f.h);
			b.establecer_posicion(40+( (x*f.w) + 2)+f.desp_x, 16+f.desp_y, f.w, f.h);
		}	
	}
	virtual std::string a_cadena() const {return "";}

	Rep_municion(unsigned int px, unsigned int pa, unsigned int offset)
		:x(px), alpha(pa), offset(offset)
	{}
	virtual ~Rep_municion() {}

	static Frame_sprites obtener_frame_hud(unsigned int offset) {return TREC.obtener(offset);}
};

#endif
