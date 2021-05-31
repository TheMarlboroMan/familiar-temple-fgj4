#include "hi_score.h"
#include "../env.h"

#include <fstream>
#include <sstream>
#include <algorithm>

hi_score::hi_score(
	unsigned int _level, 
	unsigned int _score, 
	bool _win,
	double _time
):
	level{_level},
	score{_score},
	win_game{_win},
	game_time{_time}{

}

hi_score_manager::hi_score_manager() {

	load();
}

void hi_score_manager::submit(const hi_score& _item) {

	current_scores.push_back(_item);
	std::sort(std::begin(current_scores), std::end(current_scores));

	if(current_scores.size() > 10) {

		current_scores.resize(10);
	}

	save();
}

void hi_score_manager::save() {

	std::ofstream file(get_filename());

	for(const auto& item : current_scores) {

		file<<item.score<<" "<<item.level<<" "<<(item.win_game ? 1 : 0)<<" "<<item.game_time<<std::endl;
	}
}

void hi_score_manager::load() {

	current_scores.clear();

	std::ifstream file(get_filename());

	if(!file) {

		throw std::runtime_error("no hi score table found");
	}

	std::string buff;

	while(true) {

		std::getline(file, buff);
		if(file.eof()) {

			break;
		}

		if(!buff.size()) {
	
			continue;
		}

		unsigned int score{0}, level{0};
		bool win{false};
		double time{0.0};

		std::stringstream ss{buff};
		ss>>score>>level>>win>>time;

		current_scores.push_back({level, score, win, time});
	}

	if(current_scores.size() > 10) {

		current_scores.resize(10);
	}
}

std::string hi_score_manager::get_filename() const {

	return env::usr_path+"scores.dat";
}
