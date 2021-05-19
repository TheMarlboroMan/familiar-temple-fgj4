#ifndef PATRULLADOR_H
#define PATRULLADOR_H

#include "base.h"
#include "../movil.h"
#include "../herramientas_proyecto/tabla_sprites.h"
#include "../recursos/recursos.h"

class Patrullador: public Enemigo_base, public Movil, public Representable
{
	////////////////////////
	// Definiciones...

	public:

	static float VELOCIDAD;
	static Tabla_sprites TREC;

	private:

	static const unsigned int W=8;
	static const unsigned int H=8;

	/////////////////////////
	// Propiedades...

	private:
	float angulo;
	float tiempo;

	///////////////////////////
	// Implementando Espaciable

	private:
	virtual std::string espaciable_info() const {return a_cadena();}

	///////////////////////
	// Implementación de Representable.
	public:

	virtual unsigned short int obtener_profundidad_ordenacion() const {return 30;}
	virtual void transformar_bloque(Bloque_transformacion_representable &b) const;
	virtual std::string a_cadena() const
	{
#ifdef WINCOMPIL
	using namespace parche_mingw;
#else
	using namespace std;
#endif
		return "PATRULLADOR EN "+to_string(acc_espaciable_x())+","+to_string(acc_espaciable_y());
	}

	///////////////////////
	// Implementación de Enemigo base
	public:

	virtual void recibir_visitante(Visitante_enemigo& v) {v.visitar(*this);}
	virtual bool es_saltable() const {return true;}
	virtual bool es_disparable() const {return false;}
	virtual unsigned int puntuacion_por_eliminar() const {return 50;}

	/////////////////////////
	// Métodos internos.

	private:

	////////////////////////
	// Interfaz pública...

	public:

	Patrullador();
	~Patrullador();

	void girar()
	{
		angulo+=90;
		if(angulo >= 360.0) angulo=0.0;
		establecer_vector(DLibH::Vector_2d::vector_unidad_para_angulo(angulo));
		establecer_vector(-(acc_vector().y), V_Y);
	}
	void turno(float delta) {tiempo+=(500.0 * delta);}
};

#endif
