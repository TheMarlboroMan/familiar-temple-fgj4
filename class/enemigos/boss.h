#ifndef BOSS_H
#define BOSS_H

#include "base.h"
#include "../movil.h"
#include "../herramientas_proyecto/tabla_sprites.h"
#include "../herramientas_proyecto/generador_numeros.h"
#include "../recursos/recursos.h"

#ifdef WINCOMPIL
	using namespace parche_mingw;
#else
	using namespace std;
#endif

class Boss: public Enemigo_base, public Movil, public Representable
{
	////////////////////////
	// Definiciones...

	public:

	static float VELOCIDAD;
	static Tabla_sprites TREC;

	private:

	static const unsigned int W=16;
	static const unsigned int H=16;

	/////////////////////////
	// Propiedades...

	private:
	static Generador_int GI;
	float angulo;
	float angulo_encarar;
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
		return "BOSS EN "+to_string(acc_espaciable_x())+","+to_string(acc_espaciable_y());
	}

	///////////////////////
	// Implementación de Enemigo base
	public:

	virtual void recibir_visitante(Visitante_enemigo& v) {v.visitar(*this);}
	virtual bool es_saltable() const {return true;}
	virtual bool es_disparable() const {return true;}
	virtual bool es_boss() const {return true;}
	virtual unsigned int puntuacion_por_eliminar() const {return 1000;}

	/////////////////////////
	// Métodos internos.

	private:

	////////////////////////
	// Interfaz pública...

	public:

	Boss();
	~Boss();

	float acc_angulo_encarar() const {return angulo_encarar;}

	void girar()
	{
		angulo+=90;
		if(angulo >= 360.0) angulo=0.0;
		establecer_vector(DLibH::Vector_2d::vector_unidad_para_angulo(angulo));
		establecer_vector(-(acc_vector().y), V_Y);
	}

	void turno(float delta) {tiempo-=delta;}

	void recalcular_angulo(const Espaciable& e)
	{
		angulo_encarar=DLibH::Herramientas::angulo_entre_puntos(acc_espaciable_x(), acc_espaciable_y(), e.acc_espaciable_x(), e.acc_espaciable_y());
	}

	bool debe_disparar()
	{
		bool resultado=tiempo <= 0.0;
		if(resultado) tiempo=GI();
		return resultado;
	}
};

#endif
