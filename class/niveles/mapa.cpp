#include "mapa.h"

Mapa::Mapa()
	:importado(false)
{

}

Mapa::Mapa(const std::string& nombre_fichero)
	:importado(false), total_ankh(0)
{
	importar_nivel(nombre_fichero);
}

Mapa::~Mapa()
{

}

std::vector<int> Mapa::obtener_vector_enteros(const std::string& cadena, const char separador)
{
	std::vector<std::string> valores=DLibH::Herramientas::explotar(cadena, separador);
	std::vector<int> resultado;
	resultado.reserve(valores.size());
	for(const std::string& c : valores) resultado.push_back(std::atoi(c.c_str()));
	return resultado;
}

void Mapa::importar_nivel(const std::string& nombre_fichero)
{
	DLibH::Lector_txt L(nombre_fichero, '?');

	LOG<<"INFO: Cargando nivel en fichero "<<nombre_fichero<<std::endl;

	if(!L)
	{
		LOG<<"ERROR: Imposible localizar nivel en fichero "<<nombre_fichero<<std::endl;
	}
	else
	{
		const std::string TIPO_INFO="[INFO]";
		const std::string TIPO_MAPA="[MAPA]";
		const std::string TIPO_DECO="[DECO]";
		const std::string TIPO_NOMBRE="[NOMBRE]";

		enum class t_estados {NADA, INFO, MAPA, DECO, NOMBRE};
		t_estados estado=t_estados::NADA;

		std::string linea;
		unsigned int indice=0, indice_deco=0;

		while(true)
		{
			linea=L.leer_linea();
			if(L.is_eof()) break;

			if(linea==TIPO_INFO)
			{
				LOG<<"INFO: Iniciando lectura de lineas INFO"<<std::endl;
				estado=t_estados::INFO;
			}
			else if(linea==TIPO_MAPA)
			{
				LOG<<"INFO: Iniciando lectura de lineas MAPA"<<std::endl;
				estado=t_estados::MAPA;
			}
			else if(linea==TIPO_NOMBRE)
			{
				LOG<<"INFO: Iniciando lectura de lineas NOMBRE"<<std::endl;
				estado=t_estados::NOMBRE;
			}
			else if(linea==TIPO_DECO)
			{
				LOG<<"INFO: Iniciando lectura de lineas DECO"<<std::endl;
				estado=t_estados::DECO;
			}
			else
			{
				switch(estado)
				{
					case t_estados::NADA: break;
					case t_estados::NOMBRE:
						nombre=linea;
					break;
					case t_estados::INFO:
					{
						std::vector<std::string> valores=DLibH::Herramientas::explotar(linea, ' ');
						LOG<<"nivel de "<<valores[0]<<" x "<<valores[1]<<" con "<<valores[2]<<" segundos"<<std::endl;
						nivel.dimensionar_y_reiniciar(std::atoi(valores[0].c_str()), std::atoi(valores[1].c_str()));
						deco.dimensionar_y_reiniciar(std::atoi(valores[0].c_str()), std::atoi(valores[1].c_str()) );
						segundos_finalizar=std::atoi(valores[2].c_str());
					}
					break;
					case t_estados::MAPA:
					{
						auto centrar=[this](unsigned int indice, Espaciable * e)
						{
							Nivel::Coordenadas c=nivel.indice_a_coordenadas(indice);
							e->establecer_posicion( (c.x * Celda::DIM_CELDA) + ( (Celda::DIM_CELDA-e->acc_espaciable_w()) / 2), (c.y * Celda::DIM_CELDA) + ( (Celda::DIM_CELDA- e->acc_espaciable_h()) / 2) );
						};

						for(const char c : linea)
						{
							switch(c)
							{
								case '#': nivel.actualizar_celda_por_indice(indice, Celda::T_SOLIDA); break;
								case 'x': nivel.actualizar_celda_por_indice(indice, Celda::T_LETAL); break;
								case ',': nivel.actualizar_celda_por_indice(indice, Celda::T_ENTRADA); break;
								case '.': nivel.actualizar_celda_por_indice(indice, Celda::T_SALIDA); break;

								case 'a':
								case 'b':
								case 'c':
								case 'd':
								case 'e':
								case 'f':
								{
									Item_base * i=nullptr;
									switch(c)
									{
										case 'a': i=new Item_revolver(); break;
										case 'b': i=new Item_pistola(); break;
										case 'c': i=new Item_escopeta(); break;
										case 'd': i=new Item_ankh(); ++total_ankh; break;
										case 'e': i=new Item_salud(); break;
										case 'f': i=new Item_subfusil(); break;
									}

									if(i)
									{
										centrar(indice, i);
										items.push_back(std::unique_ptr<Item_base>(i));
									}
								}
								break;

								case '1':
								case '2':
								case '3':
								case '4':
								{
									Enemigo_base * p=nullptr;

									switch(c)
									{
										case '1': p=new Perseguidor(); break;
										case '2': p=new Patrullador(); break;
										case '3': p=new Disparador(); break;
										case '4': p=new Boss(); break;
									}

									if(p)
									{
										centrar(indice, p);
										enemigos.push_back(std::unique_ptr<Enemigo_base>(p));
									}
								}
								break;

								default: break;
							}

							++indice;
						}
					}
					break;

					case t_estados::DECO:
					{
						for(const char c : linea)
						{
							if(c!=' ')
							{
								int indice_tile=0;
								if( c >= 'a') indice_tile=(int) c - (int)'a' + 26; //Lool
								else indice_tile=((int) c - (int)'A' );
								++indice_tile; //La primera no vale.
								deco.convertir_celda_a_deco(indice_deco, indice_tile);
							}
							++indice_deco;
						}
					}
					break;
				}
			}
		}

		importado=true;
	}
}

std::vector<Representable *> Mapa::obtener_actores_representables()
{
	class Vis:public Visitante_enemigo
	{
		public:
		std::vector<Representable *> resultado;

		Vis():Visitante_enemigo() {}

		virtual void visitar(Enemigo_base&) {}
		virtual void visitar(Perseguidor& e) {resultado.push_back(&e);}
		virtual void visitar(Patrullador& e) {resultado.push_back(&e);}
		virtual void visitar(Disparador& e) {resultado.push_back(&e);}
		virtual void visitar(Boss& e) {resultado.push_back(&e);}
	}vis;

	for(auto& a : enemigos) a->recibir_visitante(vis);
	for(auto& a : items) vis.resultado.push_back(a.get()); //Todos los items son "representable".

	return vis.resultado;
}
