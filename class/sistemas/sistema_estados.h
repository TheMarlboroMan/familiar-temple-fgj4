#ifndef SISTEMA_ESTADOS_H
#define SISTEMA_ESTADOS_H

class Sistema_estados
{
	public:
	enum class estados {E_JUEGO, E_TITLE, E_GAME_OVER, E_FIN_1, E_FIN_2, E_HELP};

	private:
	estados estado;
	estados estado_deseado;

	public:

	Sistema_estados()
		:estado(estados::E_TITLE), estado_deseado(estados::E_TITLE)
	{}

	void cambiar_estado(estados e) {estado_deseado=e;}
	estados acc_estado() const {return estado;}	
	estados acc_estado_deseado() const {return estado_deseado;}
	bool es_cambiar_estado() const {return estado!=estado_deseado;}
	void confirmar_cambio_estado() {estado=estado_deseado;}
};

#endif
