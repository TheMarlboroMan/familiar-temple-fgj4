#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>
#include <exception>

#include "libdan2/libDan2.h"
#include "libdan2/defDanSDL.h"

//Clases del proyecto aquí.
#include "class/controlador.h"
#include "class/controladores/controlador_juego.h"
#include "class/controladores/controlador_title.h"
#include "class/controladores/controlador_game_over.h"
#include "class/controladores/controlador_fin_1.h"
#include "class/controladores/controlador_fin_2.h"

#include "class/sistemas/sistema_estados.h"

//Declaración del log del motor en espacio global.
DLibH::Log_base LOG;

int main(int argc, char ** argv)
{
	if(DLibH::Herramientas_SDL::iniciar_SDL(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK))
	{
		//Inicializar control de logs.
		LOG.inicializar("data/logs/info.log");
		LOG.activar();

		DLibH::Log_motor::arrancar("data/logs/log_motor.log");

		//Inicializar control de argumentos.
		DLibH::Controlador_argumentos CARG(argc, argv);

		Sistema_estados SES;

		Controlador controlador(CARG);
		controlador.inicializar();

		Controlador_title CT(SES, controlador.acc_pantalla());
		Controlador_game_over CG(SES);
		Controlador_fin_1 F1(SES);
		Controlador_fin_2 F2(SES);
		Controlador_juego CJ(CARG, SES);
		Controlador_interface * CINT=&CT;

		controlador.iniciar_musica();

		while(controlador.loop(*CINT))
		{
			if(SES.es_cambiar_estado())
			{
				switch(SES.acc_estado_deseado())
				{
					case Sistema_estados::estados::E_JUEGO: CINT=&CJ; break;
					case Sistema_estados::estados::E_TITLE: CINT=&CT; break;
					case Sistema_estados::estados::E_GAME_OVER: CINT=&CG; break;
					case Sistema_estados::estados::E_FIN_1: CINT=&F1; break;
					case Sistema_estados::estados::E_FIN_2: CINT=&F2; break;
				}
				SES.confirmar_cambio_estado();
			}
		};

	}
	
	DLibH::Log_motor::finalizar();
	DLibH::Herramientas_SDL::apagar_SDL();
	

	return 0;
}
