
#include "celda.h"

//TODO TODO...
extern DLibH::Log_base LOG;
//TODO TODO

Tabla_sprites Celda::TREC("data/recortes/bloques.dat");
Tabla_sprites Celda_deco::TREC("data/recortes/tiles.dat");

Celda::Celda(unsigned int px, unsigned int py)
	:Representable(),
	x(px), y(py)
{

}

Celda::Celda(const Celda& o)
	:Representable()
{
	x=o.x;
	y=o.y;
}

Celda& Celda::operator=(const Celda& o)
{
	Representable::operator=(o);
	x=o.x;
	y=o.y;
	return *this;
}

Celda::~Celda()
{

}

unsigned short int Celda::obtener_profundidad_ordenacion() const
{
	return 10;
}

void Celda::transformar_bloque(Bloque_transformacion_representable &b) const
{
	Frame_sprites f=TREC.obtener(acc_tipo());
	if(f)
	{
		b.establecer_tipo(Bloque_transformacion_representable::TR_BITMAP);
		b.establecer_alpha(255);
		b.establecer_recurso(Recursos_graficos::RT_BLOQUES);
		b.establecer_recorte(f.x, f.y, f.w, f.h);
		b.establecer_posicion( (x*DIM_CELDA)+f.desp_x, (y*DIM_CELDA)+f.desp_y, DIM_CELDA, DIM_CELDA);
	}
}

void Celda_deco::transformar_bloque(Bloque_transformacion_representable &b) const
{
	Frame_sprites f=TREC.obtener(tile);
	if(f)
	{
		b.establecer_tipo(Bloque_transformacion_representable::TR_BITMAP);
		b.establecer_alpha(255);
		b.establecer_recurso(Recursos_graficos::RT_TILES);
		b.establecer_recorte(f.x, f.y, f.w, f.h);
		b.establecer_posicion( (acc_x()*DIM_CELDA)+f.desp_x, (acc_y()*DIM_CELDA)+f.desp_y, DIM_CELDA, DIM_CELDA);
	}
}
