#ifndef BORRABLE_H
#define BORRABLE_H

#include "../libdan2/libDan2.h"

extern DLibH::Log_base LOG;

class Borrable
{
	private:

	bool para_borrar; //Aunque no lo vayamos a usar en todos.

	public:

	Borrable():para_borrar(false) {}

	bool es_para_borrar() const {return para_borrar;}
	void marcar_para_borrar() {para_borrar=true;}
};

#endif
