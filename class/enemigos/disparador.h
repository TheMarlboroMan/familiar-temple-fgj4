#ifndef DISPARADOR_H
#define DISPARADOR_H

#include "base.h"
#include "../herramientas_proyecto/tabla_sprites.h"
#include "../herramientas_proyecto/generador_numeros.h"
#include "../recursos/recursos.h"

class Disparador: public Enemigo_base, public Representable
{
	////////////////////////
	// Definiciones...

	public:

	private:

	static const unsigned int W=16;
	static const unsigned int H=16;

	/////////////////////////
	// Propiedades...

	private:
	static Generador_int GI;
	static Tabla_sprites TREC;
	float angulo;
	float tiempo_restante;

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
		return "DISPARADOR EN "+to_string(acc_espaciable_x())+","+to_string(acc_espaciable_y());
	}

	///////////////////////
	// Implementación de Enemigo base
	public:

	virtual void recibir_visitante(Visitante_enemigo& v) {v.visitar(*this);}
	virtual bool es_saltable() const {return true;}
	virtual bool es_disparable() const {return true;}
	virtual unsigned int puntuacion_por_eliminar() const {return 150;}

	/////////////////////////
	// Métodos internos.
	
	private:

	////////////////////////
	// Interfaz pública...

	public:

	Disparador();
	~Disparador();	

	float acc_angulo() const {return angulo;}

	bool debe_disparar() 
	{
		bool resultado=tiempo_restante <= 0.0;
		if(resultado) tiempo_restante=GI();
		return resultado;
	}

	void turno(float delta) 
	{
		tiempo_restante-=delta;
	}

	void recalcular_angulo(const Espaciable& e)
	{
		angulo=DLibH::Herramientas::angulo_entre_puntos(acc_espaciable_x(), acc_espaciable_y(), e.acc_espaciable_x(), e.acc_espaciable_y());
	}
};

#endif
