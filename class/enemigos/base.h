#ifndef ENEMIGO_BASE_H
#define ENEMIGO_BASE_H

#include "../actor.h"
#include "../borrable.h"

class Enemigo_base;
class Perseguidor;
class Patrullador;
class Disparador;
class Boss;

class Visitante_enemigo
{
	public:

	Visitante_enemigo() {}

	virtual void visitar(Enemigo_base&)=0;
	virtual void visitar(Perseguidor&)=0;
	virtual void visitar(Patrullador&)=0;
	virtual void visitar(Disparador&)=0;
	virtual void visitar(Boss&)=0;
};

class Enemigo_base:public Actor, public Borrable
{
	private:

	int salud;
	static unsigned int cuenta_id;
	unsigned int id;

	///////////////////////////
	// Implementando Espaciable

	private:
	virtual std::string espaciable_info() const=0;

	public:

	Enemigo_base():Actor(0,0,0,0), salud(0), id(cuenta_id++) {}
	virtual ~Enemigo_base() {}

	bool operator!=(const Enemigo_base& e) {return id!=e.id;}

	void establecer_salud(int val) {salud=val;}
	int acc_salud() const {return salud;}
	void restar_salud(int val) {salud-=val;}	
	bool es_sin_salud() {return salud <= 0;}

	virtual void recibir_visitante(Visitante_enemigo&)=0;
	virtual bool es_saltable() const=0;
	virtual bool es_disparable() const=0;
	virtual bool es_boss() const {return false;}
	virtual unsigned int puntuacion_por_eliminar() const=0;
};

#endif
