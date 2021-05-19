#include "disparador.h"

Generador_int Disparador::GI(1, 5);

Disparador::Disparador()
	:Enemigo_base(),
	angulo(0.0), tiempo_restante(0.0)
{
	mut_w_caja(W);
	mut_h_caja(H);
	establecer_salud(100);
	tiempo_restante=GI();
}

Disparador::~Disparador()
{

}

void Disparador::transformar_bloque(Bloque_transformacion_representable &b) const
{
	b.establecer_tipo(Bloque_transformacion_representable::TR_BITMAP);
	b.establecer_alpha(255);
	b.establecer_recurso(Recursos_graficos::RT_SPRITES);

	b.rotar(angulo-90.0);

	Frame_sprites f=TREC.obtener(0);
	if(f)
	{
		b.establecer_recorte(f.x, f.y, f.w, f.h);
		b.establecer_posicion(acc_espaciable_x()+f.desp_x, acc_espaciable_y()+f.desp_y, f.w, f.h);
	}
}
