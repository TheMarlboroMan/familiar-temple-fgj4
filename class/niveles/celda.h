#ifndef CELDA_H
#define CELDA_H

#include "../graficos/representable.h"
#include "../espaciable.h"
#include "../recursos/recursos.h"
#include "../herramientas_proyecto/tabla_sprites.h"

#ifdef WINCOMPIL
	using namespace parche_mingw;
#else
	using namespace std;
#endif

class Celda:public Representable, public Espaciable
{
	///////////////////////////
	// Definiciones

	public:

	static Tabla_sprites TREC;
	static const int DIM_CELDA=32; //Dimensiones de una celda en el mundo.

	enum tipos {
		T_SIN_CELDA=0,
		T_CELDA_DECO=1,	//Realmente no vamos a verla.
		T_SOLIDA=10,
		T_LETAL=20,
		T_ENTRADA=30,
		T_SALIDA=40
	};

	private:

	//////////////
	// Propiedades

	private:

	unsigned int x; //X e Y en función de rejilla.
	unsigned int y;

	//////////////////////////////
	// Métodos internos...

	unsigned int obtener_x_mundo() const {return x*DIM_CELDA;}
	unsigned int obtener_xw_mundo() const {return (x*DIM_CELDA)+DIM_CELDA;}
	unsigned int obtener_y_mundo() const {return y*DIM_CELDA;}
	unsigned int obtener_yh_mundo() const {return (y*DIM_CELDA)+DIM_CELDA;}

	////////////////////
	// Implementando Representable...

	public:

	virtual unsigned short int obtener_profundidad_ordenacion() const;
	virtual void transformar_bloque(Bloque_transformacion_representable &b) const;
	virtual std::string a_cadena() const {return "CELDA EN "+to_string(x)+","+to_string(y)+" DEL TIPO "+to_string(acc_tipo());}

	////////////////////
	// Implementando Colisionable / Espaciable...

	virtual std::string espaciable_info() const {return a_cadena();}
	virtual Espaciable::t_caja copia_caja() const {return Espaciable::t_caja(obtener_x_mundo(), obtener_y_mundo(), DIM_CELDA, DIM_CELDA);}
	virtual void mut_x_caja(float) {}	//Prácticamente un hack.
	virtual void mut_y_caja(float) {}
	virtual void mut_w_caja(unsigned int) {}
	virtual void mut_h_caja(unsigned int) {}
	virtual void desplazar_caja(float, float) {}

	////////////////////
	// Interface pública

	public:

	unsigned int acc_x() const {return x;}
	unsigned int acc_y() const {return y;}

	void mut_x(unsigned int v) {x=v;}
	void mut_y(unsigned int v) {y=v;}

	Celda(unsigned int x, unsigned int y);
	Celda(const Celda&);
	Celda& operator=(const Celda&);
	virtual ~Celda();

	virtual unsigned int acc_tipo() const=0;

	virtual bool es_valida() const {return true;}
	virtual bool es_solida() const {return false;}
	virtual bool es_letal() const {return false;}
	virtual bool es_retiene_enemigo_tierra() const {return false;}

};

class Celda_no_valida:public Celda
{
	public:

	Celda_no_valida(unsigned int x, unsigned int y)
		:Celda(x, y)
	{}

	virtual unsigned int acc_tipo() const {return T_SIN_CELDA;}
	virtual bool es_valida() const {return false;}
};

class Celda_deco:public Celda
{
	private:

	unsigned int tile;

	public:

	static Tabla_sprites TREC;

	Celda_deco(unsigned int x, unsigned int y, unsigned int t)
		:Celda(x, y), tile(t)
	{}

	virtual unsigned int acc_tipo() const {return T_CELDA_DECO;}
	unsigned int acc_tile() const {return tile;}
	virtual void transformar_bloque(Bloque_transformacion_representable &b) const;
};

class Celda_solida:public Celda
{
	public:

	Celda_solida(unsigned int x, unsigned int y)
		:Celda(x, y)
	{}

	virtual unsigned int acc_tipo() const {return T_SOLIDA;}
	virtual bool es_solida() const {return true;}
	virtual bool es_retiene_enemigo_tierra() const {return true;}
};

class Celda_letal:public Celda
{
	public:

	Celda_letal(unsigned int x, unsigned int y)
		:Celda(x, y)
	{}

	virtual unsigned int acc_tipo() const {return T_LETAL;}
	virtual bool es_letal() const {return true;}
	virtual bool es_retiene_enemigo_tierra() const {return true;}
};

class Celda_entrada:public Celda
{
	public:

	Celda_entrada(unsigned int x, unsigned int y)
		:Celda(x, y)
	{}

	virtual unsigned int acc_tipo() const {return T_ENTRADA;}
	virtual bool es_solida() const {return false;}
	virtual bool es_retiene_enemigo_tierra() const {return false;}
};

class Celda_salida:public Celda
{
	public:

	Celda_salida(unsigned int x, unsigned int y)
		:Celda(x, y)
	{}

	virtual unsigned int acc_tipo() const {return T_SALIDA;}
	virtual bool es_solida() const {return false;}
	virtual bool es_retiene_enemigo_tierra() const {return false;}
};

class Factoria_celdas
{
	private:

	Factoria_celdas();
	~Factoria_celdas();

	public:

	//TODO: Shmart phointerx.

	static Celda * fabricar(unsigned int x, unsigned int y, unsigned int tipo)
	{
		switch(tipo)
		{
			case Celda::T_SOLIDA: 		return new Celda_solida(x, y); break;
			case Celda::T_LETAL:		return new Celda_letal(x, y); break;
			case Celda::T_ENTRADA:		return new Celda_entrada(x, y); break;
			case Celda::T_SALIDA:		return new Celda_salida(x, y); break;
			default:
				//TODO...
				return 0;
			break;
		}
	}


};
#endif
