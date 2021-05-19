#include "control_armas.h"

Control_armas::Control_armas()
	:arma_actual(t_armas::REVOLVER)
{
	iniciar();
	reset();
}

Control_armas::~Control_armas()
{

}

void Control_armas::iniciar()
{
	info_armas[t_armas::REVOLVER]=Info_municion(6, 6, 18, 0.30);
	info_armas[t_armas::PISTOLA]=Info_municion(12, 0, 0, 0.12);
	info_armas[t_armas::ESCOPETA]=Info_municion(8, 0, 0, 0.7);
}

void Control_armas::reset()
{
	arma_actual=t_armas::REVOLVER;
	info_armas[t_armas::REVOLVER].reset(6, 6, 18);
	info_armas[t_armas::ESCOPETA].reset(8, 0, 0);
	info_armas[t_armas::PISTOLA].reset(12, 0, 0);
}
