#ifndef ITEM_ESCOPETA_H
#define ITEM_ESCOPETA_H

#include "item_base.h"

class Item_escopeta:public Item_base
{
	///////////////////////////
	// Implementando Espaciable
	private:
	virtual std::string espaciable_info() const {return a_cadena();}

	///////////////////////////
	// Implementando representable...
	public:
	virtual void transformar_bloque(Bloque_transformacion_representable &b) const
	{
		b.establecer_tipo(Bloque_transformacion_representable::TR_BITMAP);
		b.establecer_alpha(255);
		b.establecer_recurso(Recursos_graficos::RT_SPRITES);

		Frame_sprites f=acc_tabla_sprites().obtener(30);
		if(f)
		{
			b.establecer_recorte(f.x, f.y, f.w, f.h);
			b.establecer_posicion(acc_espaciable_x()+f.desp_x, acc_espaciable_y()+f.desp_y, f.w, f.h);
		}
	}	
	virtual std::string a_cadena() const {return "ITEM ESCOPETA";}

	public:

	Item_escopeta():Item_base() {}
	virtual ~Item_escopeta() {}
	virtual void recibir_visitante(Visitante_item& v) {v.visitar(*this);}
	virtual unsigned int puntuacion_por_recoger() const {return 25;}
};

#endif
