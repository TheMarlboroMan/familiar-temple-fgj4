#ifndef JUGADOR_H
#define JUGADOR_H

#include <memory>
#include "actor.h"
#include "movil.h"
#include "herramientas_proyecto/tabla_sprites.h"
#include "recursos/recursos.h"
#include "jugador_estados.h"
#include "input_usuario.h"

#ifdef WINCOMPIL
	using namespace parche_mingw;
#else
	using namespace std;
#endif

class Jugador:public Actor, public Movil, public Representable
{
	////////////////////////
	// Definiciones...

	public:

	private:

	static const float SEGUNDOS_INVULNERABILIDAD;
	static const unsigned int MAX_ENERGIA=5;
	static const unsigned int ENERGIA_INICIAL=3;

	struct Info_salto
	{
		float vz;
		float z;
		Info_salto():vz(0.0), z(0.0) {}
		void reset(){vz=0.0; z=0.0;}
	};

	/////////////////////////
	// Propiedades...

	private:

	std::unique_ptr<Estado_jugador> estado;
	Input_usuario input;

	Tabla_sprites TREC;
	unsigned int direccion;
	Info_salto info_salto;
	unsigned int energia;
	float tiempo_invulnerable;

	/////////////////////////
	// Implementando Actor...

	private:

	///////////////////////////
	// Implementando Espaciable

	public:

	virtual std::string espaciable_info() const {return a_cadena();}

	///////////////////////
	// Implementación de Representable.

	public:

	virtual unsigned short int obtener_profundidad_ordenacion() const {return 50;}
	virtual void transformar_bloque(Bloque_transformacion_representable &b) const;
	virtual std::string a_cadena() const {return "JUGADOR EN "+to_string(acc_espaciable_x())+","+to_string(acc_espaciable_y());}

	/////////////////////////
	// Métodos internos.

	private:

	void cambiar_estado(Estado_jugador * e);

	////////////////////////
	// Interfaz pública...

	public:

	Jugador();
	~Jugador();

	//const Input_usuario& acc_input() const {return input;}
	void recibir_input(const Input_usuario& iu) {input=iu;}
	void turno(float delta);

	void procesar_estado(float delta);


	//Cosas que se pueden imponer desde el controlador.
	void chocar_desde_izquierda_con(const Espaciable::t_caja& c);
	void chocar_desde_derecha_con(const Espaciable::t_caja& c);
	void chocar_desde_arriba_con(const Espaciable::t_caja& c);
	void chocar_desde_abajo_con(const Espaciable::t_caja& c);
	void saltar();
	void recargar(float _t) {cambiar_estado(new Estado_jugador_recargar(_t));}

	Estado_jugador::t_estados obtener_estado() const {return estado->acc_estado();}
	void recibir_visitante_estado(Visitante_estado_jugador& v) {estado->recibir_visitante(v);}

	bool es_en_suelo() const {return estado->acc_estado()==Estado_jugador::t_estados::SUELO;}
	bool es_en_aire() const {return estado->acc_estado()==Estado_jugador::t_estados::AIRE;}
	bool puede_saltar() const {return estado->acc_estado()==Estado_jugador::t_estados::SUELO;}

	bool es_apuntando() const {return input.apuntar;}

	void detener_vector_x() {establecer_vector(0.0, Movil::V_X);}
	void detener_vector_y() {establecer_vector(0.0, Movil::V_Y);}

	void recalcular_direccion()
	{
		auto v=acc_vector();
		if(v.x || v.y)
		{
			direccion=0;
			if(v.x) direccion+=v.x > 0.0 ? D_DERECHA : D_IZQUIERDA;
			if(v.y) direccion+=v.y > 0.0 ? D_ABAJO : D_ARRIBA;
		}
	}

	unsigned int acc_direccion() const {return direccion;}
	unsigned int acc_energia() const {return energia;}
	bool es_invulnerable() const {return tiempo_invulnerable;}
	void herir()
	{
		if(energia)
		{
			--energia;
			if(energia) tiempo_invulnerable=SEGUNDOS_INVULNERABILIDAD;
		}
	}
	void recuperar_energia()
	{
		if(energia < MAX_ENERGIA) ++energia;
	}
	void reiniciar_energia() {energia=ENERGIA_INICIAL;}


};

#endif
