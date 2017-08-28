#ifndef ACTOR_H
#define ACTOR_H

#include "graficos/representable.h"
#include "espaciable.h"

/*El actor es algo que existe en el espacio de juego.
Podemos encontrar con controladores que hacen cosas y que da igual su punto
en el espacio, pero bueno, no es importante.

Ojo, es espaciable, pero NO es Colisionable.
*/

class Actor:public Espaciable
{
	////////////////////////
	// Propiedades.

	private:

	Espaciable::t_caja caja;

	////////////////////////	
	// A implementar...

	///////////////////////
	// Implementación de espaciable

	public:

	virtual std::string espaciable_info() const=0;

	virtual Espaciable::t_caja copia_caja() const {return caja;}
	virtual void mut_x_caja(float v) {caja.origen.x=v;}
	virtual void mut_y_caja(float v) {caja.origen.y=v;}
	virtual void mut_w_caja(unsigned int v) {caja.w=v;}
	virtual void mut_h_caja(unsigned int v) {caja.h=v;}
	virtual void desplazar_caja(float x, float y) {caja.origen.x+=x; caja.origen.y+=y;}

	///////////////////////
	// Interfaz pública.

	public:

	Actor(float x, float y, unsigned int w, unsigned int h);
	~Actor();
};

#endif
