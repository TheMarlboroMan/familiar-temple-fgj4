#include "patrullador.h"

float Patrullador::VELOCIDAD=60.0;

Patrullador::Patrullador()
	:Enemigo_base(), Movil(),
	angulo(0.0), tiempo(0)
{
	mut_w_caja(W);
	mut_h_caja(H);
	establecer_salud(0);
	girar();	//Establecemos el vector por primera vez.
}

Patrullador::~Patrullador()
{

}

void Patrullador::transformar_bloque(Bloque_transformacion_representable &b) const
{
	b.establecer_tipo(Bloque_transformacion_representable::TR_BITMAP);
	b.establecer_alpha(255);
	b.establecer_recurso(Recursos_graficos::RT_SPRITES);
	b.rotar(fmod(tiempo, 360.0));

	Frame_sprites f=TREC.obtener(0);
	if(f)
	{
		b.establecer_recorte(f.x, f.y, f.w, f.h);

		/*Cuando el ancho de la caja lógica sea distinto del ancho de la caja
		de representación ajustaremos la posición.*/

		if(f.w != acc_espaciable_w()) f.desp_x-=f.w-acc_espaciable_w();
		b.establecer_posicion(acc_espaciable_x()+f.desp_x, acc_espaciable_y()+f.desp_y, f.w, f.h);
	}
}
