#ifndef MAPA_H
#define MAPA_H

#include <exception>
#include <memory>
#include "nivel.h"
#include "../../libdan2/libDan2.h"

#include "../enemigos/perseguidor.h"
#include "../enemigos/patrullador.h"
#include "../enemigos/disparador.h"
#include "../enemigos/boss.h"

#include "../items/item_revolver.h"
#include "../items/item_pistola.h"
#include "../items/item_escopeta.h"
#include "../items/item_ankh.h"
#include "../items/item_salud.h"
#include "../items/item_subfusil.h"

/*La clase mapa es el conglomerado de la geometría del nivel y cualquier
otra cosa que pueda contener. El cargador de mapas crearía un mapa (aunque
la interfaz no impide que lo haga el controlador) y eso es lo que el controlador
maneja.
*/

extern DLibH::Log_base LOG;

struct Excepcion_mapa
{
	enum class tipos {ENTRADA_INVALIDA};
	tipos tipo;
	Excepcion_mapa(tipos t):tipo(t){}
};

class Mapa
{
	///////////////
	// Interfaz pública

	public:

	Mapa();
	Mapa(const std::string& nombre_fichero);
	Mapa(Mapa&& m):
		nivel(std::move(m.nivel)),
		deco(std::move(m.deco)),
		enemigos(std::move(m.enemigos)),
		items(std::move(m.items)),
		importado(m.importado),
		nombre(std::move(m.nombre)),
		segundos_finalizar(m.segundos_finalizar),
		total_ankh(m.total_ankh)
	{}

	Mapa& operator=(Mapa&& m)
	{
		nivel=std::move(m.nivel);
		deco=std::move(m.deco);
		importado=m.importado;
		enemigos=std::move(m.enemigos);
		items=std::move(m.items);
		nombre=std::move(m.nombre);
		segundos_finalizar=m.segundos_finalizar;
		total_ankh=m.total_ankh;
		return *this;
	}

	~Mapa();


	Nivel::Coordenadas obtener_coordenadas_inicio_en_rejilla() const
	{
		try
		{
			return nivel.obtener_coordenadas_inicio_en_rejilla();
		}
		catch(std::exception& e)
		{
			throw Excepcion_mapa(Excepcion_mapa::tipos::ENTRADA_INVALIDA);
		}
	}

	bool es_valido() const {return importado;}
	const Nivel& acc_nivel() const {return nivel;}
	Nivel& acc_nivel() {return nivel;}
	Nivel& acc_deco() {return deco;}
	std::vector<Representable *> obtener_actores_representables();
	std::vector<std::unique_ptr<Enemigo_base>>& acc_enemigos() {return enemigos;}
	std::vector<std::unique_ptr<Item_base>>& acc_items() {return items;}
	const std::string& acc_nombre() const {return nombre;}
	unsigned int acc_segundos_finalizar() const {return segundos_finalizar;}
	unsigned int acc_ankh_nivel() const {return total_ankh;}

	///////////////
	// Propiedades

	private:

	Nivel nivel;
	Nivel deco;
	std::vector<std::unique_ptr<Enemigo_base>> enemigos;
	std::vector<std::unique_ptr<Item_base>> items;
	bool importado;
	std::string nombre;
	unsigned int segundos_finalizar;
	unsigned int total_ankh;

	///////////////
	// Métodos

	private:
	Mapa(const Mapa&)=delete;
	std::vector<int> obtener_vector_enteros(const std::string& cadena, const char separador);
	void importar_nivel(const std::string& nombre_fichero);
};

#endif
