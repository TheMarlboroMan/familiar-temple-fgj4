#ifndef CONTROLADOR_TITLE
#define CONTROLADOR_TITLE

#include "controlador_interface.h"
#include "../recursos/recursos.h"
#include "../sistemas/hi_score.h"
#include "../linear_timed_function.h"

class Controlador_title:public Controlador_interface
{
	private:

	void loop_animate_logo(const Input& input, float delta);	
	bool loop_title(const Input& input, float delta);	

	static const std::string cadena_intro;
	static const std::string cadena_controles;
	std::string hi_score_string;

	hi_score_manager& hi_scores;

	DLibV::Representacion_texto_auto_estatica txt_intro;
	DLibV::Representacion_texto_auto_estatica txt_controles;
	DLibV::Representacion_texto_auto_estatica txt_version;
	DLibV::Representacion_texto_auto_estatica txt_hi_score;
	
	DLibV::Representacion_bitmap_estatica game_logo_top;
	DLibV::Representacion_bitmap_estatica game_logo_bottom;

	tools::linear_timed_function<float> ltf_logo_top,
										ltf_logo_bottom;
										
	bool top_done{false}, bottom_done{false};
	int logo_top_x{20}, logo_bottom_x{110}, logo_top_h{117}, logo_bottom_h{127};
	

	enum class states {
		animate_logo,
		title
	} state{states::animate_logo};

	public:

	Controlador_title(Sistema_estados& s, hi_score_manager&, DLibV::Pantalla&p);
	
	void    load_hi_scores();

	virtual void preloop(const Input& input, float delta) {}
	virtual void postloop() {}

	virtual bool loop(const Input& input, float delta);
	virtual void dibujar(DLibV::Pantalla& pantalla);
};

#endif
