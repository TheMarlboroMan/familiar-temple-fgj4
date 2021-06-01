#ifndef RECURSOS_PROYECTO_H
#define RECURSOS_PROYECTO_H

class Recursos_graficos
{
	private:

	Recursos_graficos();

	public:

	enum superficies{
		RS_FUENTE_BASE=1,
		RS_FONT_RED=2,
		RS_FONT_LARGE=3
	};

	enum texturas{
		RT_BLOQUES=1,
		RT_SPRITES=2,
		RT_HUD=3,
		RT_TILES=4,
		RT_TITLEPIC=5,
		RT_GAMEOVERPIC=6,
		RT_END1PIC=7,
		RT_END2PIC=8,
		RT_EVENT_LOGO=9,
		RT_GAME_LOGO=10
	};
};

class Recursos_audio
{
	private:

	Recursos_audio();

	public:

	enum musicas{
		RM_MAIN=1
	};

	enum sonidos{
		RS_DEFECTO=1,
		RS_REVOLVER=10,
		RS_PISTOLA=11,
		RS_ESCOPETA=12,
		RS_RECARGAR=13,
		RS_DISPARO_ENEMIGO=14,
		RS_SALTO=15,
		RS_PERDER_ENERGIA=16,
		RS_ARMA_VACIA=17,
		RS_VIDA_EXTRA=18,
		RS_PERDER_VIDA=19,
		RS_DESTRUIR_ENEMIGO=20
	};
};

#endif
