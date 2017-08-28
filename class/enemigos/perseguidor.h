#ifndef PERSEGUIDOR_H
#define PERSEGUIDOR_H

#include "base.h"
#include "../movil.h"
#include "../herramientas_proyecto/tabla_sprites.h"
#include "../recursos/recursos.h"

#ifdef WINCOMPIL
	using namespace parche_mingw;
#else
	using namespace std;	
#endif

class Perseguidor: public Enemigo_base, public Movil, public Representable
{
	////////////////////////
	// Definiciones...

	public:

	static float VELOCIDAD;

	private:

	static const unsigned int W=8;
	static const unsigned int H=8;

	/////////////////////////
	// Propiedades...

	private:
	static Tabla_sprites TREC;
	int salud;
	float angulo;

	///////////////////////////
	// Implementando Espaciable

	private:
	virtual std::string espaciable_info() const {return a_cadena();}

	///////////////////////
	// Implementación de Representable.
	public:

	virtual unsigned short int obtener_profundidad_ordenacion() const {return 30;}
	virtual void transformar_bloque(Bloque_transformacion_representable &b) const;
	virtual std::string a_cadena() const {return "PERSEGUIDOR EN "+to_string(acc_espaciable_x())+","+to_string(acc_espaciable_y());}

	///////////////////////
	// Implementación de Enemigo base
	public:

	virtual void recibir_visitante(Visitante_enemigo& v) {v.visitar(*this);}
	virtual bool es_saltable() const {return true;}
	virtual bool es_disparable() const {return true;}
	virtual unsigned int puntuacion_por_eliminar() const {return 0;}

	/////////////////////////
	// Métodos internos.
	
	private:

	////////////////////////
	// Interfaz pública...

	public:

	Perseguidor();
	~Perseguidor();	

	void turno(float) {}

	void recalcular_angulo(const Espaciable& e)
	{
		angulo=DLibH::Herramientas::angulo_entre_puntos(acc_espaciable_x(), acc_espaciable_y(), e.acc_espaciable_x(), e.acc_espaciable_y());
		establecer_vector(DLibH::Vector_2d::vector_unidad_para_angulo(angulo));
		establecer_vector(-(acc_vector().y), V_Y);
	}
};

#endif
