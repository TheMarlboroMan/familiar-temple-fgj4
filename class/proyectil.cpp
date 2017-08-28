#include "proyectil.h"

Tabla_sprites Proyectil::TREC("data/recortes/proyectiles.dat");

Proyectil::Proyectil(float angulo, float v, unsigned int pot, tfaccion fac)
	:Actor(0.0, 0.0, 0.0, 0.0), Movil(), 
	velocidad(v), potencia(pot), faccion(fac)
{
	establecer_vector(DLibH::Vector_2d::vector_unidad_para_angulo(angulo));
	establecer_vector(-acc_vector().y, V_Y);	//Darle la vuelta de cartesiano a SDL.

	switch(faccion)
	{
		case tfaccion::JUGADOR: 
			mut_w_caja(W_JUGADOR); 
			mut_h_caja(W_JUGADOR); 
		break;

		case tfaccion::ENEMIGO: 
			mut_w_caja(W_ENEMIGO); 
			mut_h_caja(W_ENEMIGO); 
		break;
	}
}

Proyectil::~Proyectil()
{
}

void Proyectil::transformar_bloque(Bloque_transformacion_representable &b) const
{
	b.establecer_tipo(Bloque_transformacion_representable::TR_BITMAP);
	b.establecer_alpha(255);
	b.establecer_recurso(Recursos_graficos::RT_SPRITES);
	
	if(faccion==tfaccion::ENEMIGO) b.rotar( fmod(tiempo, 360.0) );
	unsigned int indice=faccion==tfaccion::ENEMIGO ? 1 : 0;

	Frame_sprites f=TREC.obtener(indice);
	if(f)
	{
		b.establecer_recorte(f.x, f.y, f.w, f.h);
		b.establecer_posicion(acc_espaciable_x()+f.desp_x, acc_espaciable_y()+f.desp_y, f.w, f.h);
	}
}
