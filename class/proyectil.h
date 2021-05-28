#ifndef PROYECTIL_H
#define PROYECTIL_H

#include "actor.h"
#include "movil.h"
#include "borrable.h"
#include "herramientas_proyecto/tabla_sprites.h"
#include "recursos/recursos.h"

#ifdef WINCOMPIL
	using namespace parche_mingw;
#else
	using namespace std;
#endif

class Proyectil: public Actor, public Borrable, public Movil, public Representable
{
	////////////////////////
	// Definiciones...

	public:

	enum class tfaccion {JUGADOR, ENEMIGO};
	static Tabla_sprites TREC;

	private:

	static const unsigned int W_JUGADOR=5;
	static const unsigned int H_JUGADOR=5;

	static const unsigned int W_ENEMIGO=8;
	static const unsigned int H_ENEMIGO=8;

	float velocidad;
	unsigned int potencia,
				multiplier;
	tfaccion faccion;
	float tiempo;

	/////////////////////////
	// Propiedades...

	private:

	///////////////////////////
	// Implementando Espaciable

	private:
	virtual std::string espaciable_info() const {return a_cadena();}

	///////////////////////
	// Implementación de Representable.
	public:

	virtual unsigned short int obtener_profundidad_ordenacion() const {return 25;}
	virtual void transformar_bloque(Bloque_transformacion_representable &b) const;
	virtual std::string a_cadena() const {return "PROYECTIL EN "+to_string(acc_espaciable_x())+","+to_string(acc_espaciable_y());}

	/////////////////////////
	// Métodos internos.

	private:

	////////////////////////
	// Interfaz pública...

	public:

	Proyectil(float angulo, float velocidad, unsigned int, tfaccion, int _multiplier=1);
	~Proyectil();

	unsigned int acc_potencia() const {return potencia * multiplier;}

	void turno(float delta)
	{
		tiempo+=(1000.0 * delta);
		auto v=acc_vector();
		desplazar_caja(v.x * delta * velocidad, v.y * delta * velocidad);
	}
};

#endif
