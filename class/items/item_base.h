#ifndef ITEM_BASE_H
#define ITEM_BASE_H

#include "../actor.h"
#include "../borrable.h"
#include "../herramientas_proyecto/tabla_sprites.h"
#include "../recursos/recursos.h"

class Item_base;
class Item_pistola;
class Item_revolver;
class Item_escopeta;
class Item_subfusil;
class Item_ankh;
class Item_salud;

class Visitante_item
{
	public:

	Visitante_item() {}

	virtual void visitar(Item_base&)=0;
	virtual void visitar(Item_pistola&)=0;
	virtual void visitar(Item_revolver&)=0;
	virtual void visitar(Item_escopeta&)=0;
	virtual void visitar(Item_subfusil&)=0;
	virtual void visitar(Item_ankh&)=0;
	virtual void visitar(Item_salud&)=0;
};

class Item_base:public Actor, public Borrable, public Representable
{
	/////////////////////////
	// Propiedades...
	private:

	static const unsigned int W=16;
	static const unsigned int H=16;

	///////////////////////////
	// Implementando Espaciable
	private:
	virtual std::string espaciable_info() const=0;

	///////////////////////////
	// Implementando representable...
	public:
	virtual unsigned short int obtener_profundidad_ordenacion() const {return 15;}
	virtual void transformar_bloque(Bloque_transformacion_representable &b) const=0;
	virtual std::string a_cadena() const=0;

	public:

	Item_base():Actor(0,0,W,H) {}
	virtual ~Item_base() {}
	virtual void recibir_visitante(Visitante_item&)=0;
	virtual unsigned int puntuacion_por_recoger() const=0;
	const Tabla_sprites& acc_tabla_sprites() const {return TREC;}
	static Tabla_sprites TREC;
};

#endif
