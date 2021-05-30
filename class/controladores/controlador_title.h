#ifndef CONTROLADOR_TITLE
#define CONTROLADOR_TITLE

#include "controlador_interface.h"
#include "../recursos/recursos.h"
#include "../sistemas/hi_score.h"

class Controlador_title:public Controlador_interface
{
	private:

	static const std::string cadena_intro;
	static const std::string cadena_controles;
	static const std::string cadena_creditos;
	std::string hi_score_string;

	hi_score_manager& hi_scores;

	DLibV::Representacion_texto_auto_estatica txt_intro;
	DLibV::Representacion_texto_auto_estatica txt_controles;
	DLibV::Representacion_texto_auto_estatica txt_creditos;
	DLibV::Representacion_texto_auto_estatica txt_hi_score;

	public:

	Controlador_title(Sistema_estados& s, hi_score_manager&, DLibV::Pantalla&p);

	void    load_hi_scores();

	virtual void preloop(const Input& input, float delta) {}
	virtual void postloop() {}

	virtual bool loop(const Input& input, float delta);
	virtual void dibujar(DLibV::Pantalla& pantalla);
};

#endif
