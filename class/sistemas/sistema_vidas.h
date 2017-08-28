#ifndef SISTEMA_VIDAS_H
#define SISTEMA_VIDAS_H

class Sistema_vidas
{
	private:

	unsigned int vidas;
	const static unsigned int VIDAS_INICIO=3;

	public:

	Sistema_vidas()
		:vidas(VIDAS_INICIO)
	{}

	void restar_vida() {--vidas;}
	void sumar_vida() {++vidas;}
	bool es_quedan_vidas() const {return vidas;}
	unsigned int acc_vidas() const {return vidas;}

	void reset() {vidas=VIDAS_INICIO;}
};

#endif
