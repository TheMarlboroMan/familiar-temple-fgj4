#include "boss.h"
#include "../env.h"


Generador_int Boss::GI(1, 3);
float Boss::VELOCIDAD=90.0;

Boss::Boss()
	:Enemigo_base(), Movil(),
	angulo(0.0), angulo_encarar(0.0), tiempo(0)
{
	mut_w_caja(W);
	mut_h_caja(H);
	establecer_salud(300);
	girar();	//Establecemos el vector por primera vez.
	tiempo=GI();
}

Boss::~Boss()
{

}

void Boss::transformar_bloque(Bloque_transformacion_representable &b) const
{
	b.establecer_tipo(Bloque_transformacion_representable::TR_BITMAP);
	b.establecer_alpha(255);
	b.establecer_recurso(Recursos_graficos::RT_SPRITES);
	b.rotar(angulo_encarar-90);

	Frame_sprites f=TREC.obtener(0);
	if(f)
	{
		b.establecer_recorte(f.x, f.y, f.w, f.h);
		b.establecer_posicion(acc_espaciable_x()+f.desp_x, acc_espaciable_y()+f.desp_y, f.w, f.h);
	}
}
