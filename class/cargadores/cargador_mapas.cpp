#include "cargador_mapas.h"
#include "../env.h"

Cargador_mapas::Cargador_mapas(const std::string& ruta_listado)
{
	cargar(ruta_listado);
}

Cargador_mapas::~Cargador_mapas()
{

}

void Cargador_mapas::cargar(const std::string& ruta_listado)
{
	LOG<<"INFO: Localizando lista de niveles en "<<ruta_listado<<std::endl;
	DLibH::Lector_txt L(ruta_listado, '#');

	if(!L)
	{
		LOG<<"ERROR: No se localiza lista de niveles en "<<ruta_listado<<std::endl;
	}
	else
	{
		std::string linea;
		const char separador='\t';

		while(true)
		{
			linea=L.leer_linea();
			if(L.is_eof()) break;

			std::vector<std::string> valores=DLibH::Herramientas::explotar(linea, separador);
			if(valores.size()!=2)
			{
				LOG<<"ERROR: En lista de niveles la linea no tiene dos partes: "<<linea<<std::endl;
			}
			else
			{
				unsigned int indice=std::atoi(valores[0].c_str());

				if(niveles.count(indice))
				{
					LOG<<"ERROR: En lista de niveles el indice "<<indice<<" aparece más de una vez."<<std::endl;
				}
				else
				{
					niveles[indice]=env::make_data_path(valores[1]);
				}
			}
		}
	}

	LOG<<"INFO: Finalizada carga de "<<niveles.size()<<" niveles."<<std::endl;
}

Mapa Cargador_mapas::fabricar_mapa_indice(unsigned int indice) const
{
	if(niveles.count(indice))
	{
		return Mapa(niveles.at(indice));
	}
	else
	{
		throw Excepcion_cargador_mapas(Excepcion_cargador_mapas::tipos::INDICE_INCORRECTO);
	}
}
