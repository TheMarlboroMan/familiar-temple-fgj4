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
	info_armas[t_armas::REVOLVER]=Info_municion(6, 6, 18, 0.30, 1.2, false);
	info_armas[t_armas::PISTOLA]=Info_municion(12, 0, 0, 0.12, 1.0, false);
	info_armas[t_armas::ESCOPETA]=Info_municion(8, 0, 0, 0.7, 1.5, false);
	info_armas[t_armas::SUBFUSIL]=Info_municion(30, 0, 0, 0.1, 2.8, true);
}

void Control_armas::reset()
{
	arma_actual=t_armas::REVOLVER;
	info_armas[t_armas::REVOLVER].reset(6, 6, 18);
	info_armas[t_armas::ESCOPETA].reset(8, 0, 0);
	info_armas[t_armas::PISTOLA].reset(12, 0, 0);
	info_armas[t_armas::SUBFUSIL].reset(30, 0, 0);
}
