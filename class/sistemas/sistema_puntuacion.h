#ifndef SISTEMA_PUNTUACION_H
#define SISTEMA_PUNTUACION_H

class Sistema_puntuacion
{
	private:

	unsigned int puntuacion;
	unsigned int siguiente_vida;
	unsigned int todos_ankh;
	unsigned int ankh_nivel;

	bool reclamar_vida;

	static const unsigned int PUNTOS_PARA_VIDA=1500;

	public:

	Sistema_puntuacion()
		:puntuacion(0), siguiente_vida(PUNTOS_PARA_VIDA), todos_ankh(0), ankh_nivel(0), reclamar_vida(false)
	{}
	
	void sumar_puntuacion(unsigned int p)
	{
		puntuacion+=p;
		if(puntuacion >= siguiente_vida)
		{
			siguiente_vida+=PUNTOS_PARA_VIDA;
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
	}
};

#endif
