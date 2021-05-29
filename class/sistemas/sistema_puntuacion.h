#ifndef SISTEMA_PUNTUACION_H
#define SISTEMA_PUNTUACION_H

#include <iostream>
#include "../../libdan2/libDan2.h"

extern DLibH::Log_base LOG;

class Sistema_puntuacion
{
	private:

	unsigned int puntuacion;
	unsigned int siguiente_vida;
	unsigned int todos_ankh;
	unsigned int ankh_nivel;

	bool reclamar_vida;

	static const unsigned int PUNTOS_PARA_VIDA=1500;
	int next_life_multiplier{1};

	public:

	Sistema_puntuacion()
		:puntuacion(0), siguiente_vida(PUNTOS_PARA_VIDA), todos_ankh(0), ankh_nivel(0), reclamar_vida(false)
	{}
	
	void sumar_puntuacion(unsigned int p)
	{
		puntuacion+=p;
		if(puntuacion >= siguiente_vida) {


			if(next_life_multiplier <= 4) {
				++next_life_multiplier;
				siguiente_vida+=next_life_multiplier*PUNTOS_PARA_VIDA;
				LOG<<"next life with multiplier at "<<siguiente_vida<<std::endl;
			}
			else {

				siguiente_vida+=(PUNTOS_PARA_VIDA*5);
				LOG<<"next life static at "<<siguiente_vida<<std::endl;
			}

			reclamar_vida=true;
		}
	}

	void recoger_ankh() {++ankh_nivel;}
	void marcar_nivel_como_todos_ankh() {++todos_ankh;}
	void reset_ankh_nivel() {ankh_nivel=0;}

	void marcar_vida_como_reclamada() {reclamar_vida=false;}

	unsigned int acc_puntuacion() const {return puntuacion;}
	unsigned int acc_ankh_nivel() const {return ankh_nivel;}
	unsigned int acc_todos_ankh() const {return todos_ankh;}
	bool es_vida_extra() const {return reclamar_vida;}

	void reset()
	{
		ankh_nivel=0;
		puntuacion=0;
		siguiente_vida=PUNTOS_PARA_VIDA;
		next_life_multiplier=1;
	}
};

#endif
