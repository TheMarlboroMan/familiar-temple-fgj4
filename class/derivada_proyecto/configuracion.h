#ifndef PROYECTO_SDL2_DERIVADA_CONFIGURACION_H
#define PROYECTO_SDL2_DERIVADA_CONFIGURACION_H

#include "../base_proyecto/configuracion_base.h"
#include "../env.h"

class Configuracion: public Configuracion_base
{
	////////////////////////////////////
	// Definiciones...

	static const std::string CLAVE_IDIOMA;

	////////////////////////////////////
	// Implementacion...

	protected:

	virtual void grabar_valores_configuracion(std::ofstream&, const std::string&);
	virtual void asignar_valores_por_defecto();
	virtual void procesar_clave_y_valor(const std::string&, const std::string&);

	std::string obtener_ruta_archivo() const {return env::usr_path+"configuracion.dat";}
	std::string obtener_separador_archivo() const {return ":";}
	std::string obtener_clave_version_archivo() const {return "v";}
	std::string obtener_version_archivo() const {return "1";}
	std::string obtener_clave_pantalla_completa() const {return "pantalla_completa";}
	std::string obtener_clave_modo_hardware() const {return "modo_hardware";}
	std::string obtener_clave_pantalla_doble_buffer() const {return "doble_buffer";}
	std::string obtener_clave_pantalla_anyformat() const {return "pantalla_anyformat";}
	std::string obtener_clave_volumen_audio() const {return "volumen_audio";}
	std::string obtener_clave_volumen_musica() const {return "volumen_musica";}
	std::string obtener_clave_audio_ratio() const {return "audio_ratio";}
	std::string obtener_clave_audio_buffers() const {return "audio_buffers";}
	std::string obtener_clave_audio_salidas() const {return "audio_salidas";}
	std::string obtener_clave_audio_canales() const {return "audio_canales";}
	unsigned short int valor_pantalla_completa_defecto() const {return 0;}
	unsigned short int valor_modo_hardware_defecto() const {return 1;}
	unsigned short int valor_pantalla_doble_buffer_defecto() const {return 1;}
	unsigned short int valor_pantalla_anyformat_defecto() const {return 1;}
	unsigned short int valor_volumen_audio_defecto() const {return 64;}
	unsigned short int valor_volumen_musica_defecto() const {return 64;}
	int valor_audio_ratio_defecto() const {return 44100;}
	int valor_audio_salidas_defecto() const {return 2;}
	int valor_audio_buffers_defecto() const {return 1024;}
	int valor_audio_canales_defecto() const {return 8;}

	///////////////////////////////////
	// Propiedades.

	private:

	unsigned int idioma;
	int gamepad_up{0};
	int gamepad_right{0};
	int gamepad_down{0};
	int gamepad_left{0};
	int gamepad_jump{0};
	int gamepad_fire{0};
	int gamepad_strafe{0};
	int gamepad_reload{0};
	int gamepad_focus{0};
	int gamepad_discard_weapon{0};

	////////////////////////////////
	// Interface pública.

	public:

	unsigned short int acc_idioma() const {return this->idioma;}
	int get_gamepad_up() const {return gamepad_up;}
	int get_gamepad_right() const {return gamepad_right;}
	int get_gamepad_down() const {return gamepad_down;}
	int get_gamepad_left() const {return gamepad_left;}
	int get_gamepad_jump() const {return gamepad_jump;}
	int get_gamepad_fire() const {return gamepad_fire;}
	int get_gamepad_strafe() const {return gamepad_strafe;}
	int get_gamepad_reload() const {return gamepad_reload;}
	int get_gamepad_focus() const {return gamepad_focus;}
	int get_gamepad_discard_weapon() const {return gamepad_discard_weapon;}
	void mut_idioma(unsigned short int p_valor) {this->idioma=p_valor;}

	Configuracion();
	~Configuracion();
};

#endif
