#pragma once

#include "controlador_interface.h"
#include "../recursos/recursos.h"
#include "../linear_timed_function.h"

class logos:public Controlador_interface
{
	public:

	logos(Sistema_estados& s, DLibV::Pantalla&p);

	virtual void preloop(const Input& input, float delta) {}
	virtual void postloop() {}

	virtual bool loop(const Input& input, float delta);
	virtual void dibujar(DLibV::Pantalla& pantalla);
	
	private:
	
	void        run_logic(float);
	
	
	DLibV::Representacion_texto_auto_estatica made_for;
	DLibV::Representacion_bitmap_estatica event_logo;
	DLibV::Representacion_texto_auto_estatica txt_author;
	
	
	float       time{0.f};
	tools::linear_timed_function<float> ltf;
	enum class steps {
		start,
		show_event,
		wait,
		show_author,
		wait_again,
		fadeout,
		end
	}           step{steps::start};
};

