#ifndef REPRESENTACION_AGRUPADA_H
#define REPRESENTACION_AGRUPADA_H

#include "../representacion.h"
#include "../../../herramientas/herramientas_sdl/herramientas_sdl.h"
#include <vector>

namespace DLibV
{

/*Tiene un vector de varias representaciones. Cuando se lo pasamos a la pantalla
las vuelca en el orden en que se han insertado. Las representacions deben
ser estáticas  o dinámicas, según el tipo de grupo. 

Una vez insertadas, el grupo toma posesión de las representaciones y se encarga 
de destruirlas al finalizar su tiempo de vida.*/

class Representacion_agrupada:public Representacion
{
	private:

	bool posee_las_representaciones;
	bool impone_alpha;
	bool impone_modo_blend;
	Representacion_agrupada& operator=(const Representacion_agrupada &);	//OJO: Es incopiable!.

	protected:

	std::vector<Representacion *> grupo;

	bool volcado(SDL_Renderer *);
	bool volcado(SDL_Renderer *, const SDL_Rect&, const SDL_Rect&, int=0, int=0);

	public:

	bool es_preparada() const {return true;}

	Representacion_agrupada(bool=true);
	Representacion_agrupada(const Representacion_agrupada&);
	virtual ~Representacion_agrupada();
	virtual bool es_estatica() const=0;
	virtual bool insertar_representacion(Representacion *);

	void imponer_alpha() {impone_alpha=true;}
	void no_imponer_alpha() {impone_alpha=false;}
	
	void imponer_modo_blend() {impone_modo_blend=true;}
	void no_imponer_modo_blend() {impone_modo_blend=false;}

	void poseer_representaciones() {this->posee_las_representaciones=true;}
	void no_poseer_representaciones() {this->posee_las_representaciones=false;}
	
	void vaciar_grupo();

	unsigned int obtener_cuenta() const {return grupo.size();}
};

class Representacion_agrupada_dinamica:public Representacion_agrupada
{
	private:

	Representacion_agrupada_dinamica& operator=(const Representacion_agrupada_dinamica &p_otra);

	public:

	Representacion_agrupada_dinamica(bool poseer):Representacion_agrupada(poseer) {}
	virtual ~Representacion_agrupada_dinamica() {}
	virtual bool es_estatica() const {return false;}

	bool insertar_representacion(Representacion * p_rep)
	{
		if(p_rep->es_estatica()) 
		{
			return false;
		}
		else 
		{
			return Representacion_agrupada::insertar_representacion(p_rep);
		}
	}
};

class Representacion_agrupada_estatica:public Representacion_agrupada
{
	private:

	Representacion_agrupada_estatica& operator=(const Representacion_agrupada_estatica &p_otra);

	public:

	Representacion_agrupada_estatica(bool poseer):Representacion_agrupada(poseer) {}
	virtual ~Representacion_agrupada_estatica() {}
	virtual bool es_estatica() const {return true;}

	bool insertar_representacion(Representacion * p_rep)
	{
		if(!p_rep->es_estatica()) 
		{
			return false;
		}
		else 
		{
			return Representacion_agrupada::insertar_representacion(p_rep);
		}
	}
};

}

#endif
