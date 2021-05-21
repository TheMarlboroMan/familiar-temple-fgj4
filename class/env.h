#pragma once

#include <string>
#include <iostream>

struct env {

	public:

	static const int major_version{1};
	static const int minor_version{1};
	static const int patch_version{1};
	static std::string usr_path;

	static std::string make_data_path(const std::string _path) {

		if(!data_path.size()) {

			throw std::runtime_error("data path not loaded");
		}

		std::string str=data_path+_path;

		return str;
	}

	static void set_data_path(const std::string& _path) {

		data_path=_path;
	}


	private:

	static std::string data_path;
};
