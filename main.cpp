#include <iostream>

#include <vector>
#include <map>
#include <cmath>
#include <algorithm>
#include <exception>
#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include "libdan2/libDan2.h"
#include "libdan2/defDanSDL.h"

//Clases del proyecto aquí.
#include "class/env.h"
#include "class/controlador.h"
#include "class/controladores/controlador_juego.h"
#include "class/controladores/controlador_title.h"
#include "class/controladores/controlador_game_over.h"
#include "class/controladores/controlador_fin_1.h"
#include "class/controladores/controlador_fin_2.h"

#include "class/sistemas/sistema_estados.h"

//Declaración del log del motor en espacio global.
DLibH::Log_base LOG;

std::string env::data_path="";
std::string env::usr_path="";
Tabla_sprites Proyectil::TREC=Tabla_sprites();
Tabla_sprites Boss::TREC=Tabla_sprites();
Tabla_sprites Disparador::TREC=Tabla_sprites();
Tabla_sprites Patrullador::TREC=Tabla_sprites();
Tabla_sprites Perseguidor::TREC=Tabla_sprites();
Tabla_sprites Item_base::TREC=Tabla_sprites();
Tabla_sprites Celda::TREC=Tabla_sprites();
Tabla_sprites Celda_deco::TREC=Tabla_sprites();
Tabla_sprites Rep_municion::TREC=Tabla_sprites();

int main(int argc, char ** argv)
{
	{
		std::string executable_path, executable_dir;
		std::array<char, 1024> buff;

		int bytes=readlink("/proc/self/exe", buff.data(), 1024);
		if(-1==bytes) {

			std::cerr<<"could not locate proc/self/exe, error "<<errno<<std::endl;
			return 1;
		}

		executable_path=std::string{std::begin(buff), std::begin(buff)+bytes};
		auto last_slash=executable_path.find_last_of("/");
		executable_dir=executable_path.substr(0, last_slash)+"/";

		env::data_path=executable_dir+"/";
		env::usr_path=std::string{getenv("HOME")}+"/.familiar_temple/";

#ifdef AS_APPIMAGE
#pragma message ("Building as AppImage!!")
		env::data_path=App::env::data_path+"../share/";
#endif

		struct stat st={0};
		if(stat(env::usr_path.c_str(), &st) == -1) {

			mkdir(env::usr_path.c_str(), 0700);

			std::string logs_path=env::usr_path+"logs";
			mkdir(logs_path.c_str(), 0700);

			auto dump_file=[](const std::string _in, const std::string _out) {

				std::string res, lin;
				std::ifstream f{_in};
				std::ofstream out{_out};

				while(true) {
					std::getline(f, lin);
					if(f.eof()) {
						break;
					}
					out<<lin<<std::endl;
				}
			};

			dump_file(
				env::data_path+"data/config/configuracion.dat",
				env::usr_path+"/configuracion.dat"
			);
		}
	}

	Proyectil::TREC.cargar(env::data_path+"data/recortes/proyectiles.dat");
	Boss::TREC.cargar(env::data_path+"data/recortes/enemigo_boss.dat");
	Disparador::TREC.cargar(env::data_path+"data/recortes/enemigo_disparador.dat");
	Patrullador::TREC.cargar(env::data_path+"data/recortes/enemigo_patrullador.dat");
	Perseguidor::TREC.cargar(env::data_path+"data/recortes/enemigo_perseguidor.dat");
	Item_base::TREC.cargar(env::data_path+"data/recortes/items.dat");
	Celda::TREC.cargar(env::data_path+"data/recortes/bloques.dat");
	Celda_deco::TREC.cargar(env::data_path+"data/recortes/tiles.dat");
	Rep_municion::TREC.cargar(env::data_path+"data/recortes/hud.dat");

	if(DLibH::Herramientas_SDL::iniciar_SDL(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK))
	{

		//Inicializar control de logs.

		std::string info_log_path=env::usr_path+"data/logs/info.log";
		LOG.inicializar(info_log_path.c_str());
		LOG.activar();

		std::string motor_log_path=env::usr_path+"data/logs/log_motor.log";
		DLibH::Log_motor::arrancar(motor_log_path.c_str());

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
