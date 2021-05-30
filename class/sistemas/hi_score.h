#pragma once

#include <string>
#include <vector>

struct hi_score {

	               hi_score(unsigned int, unsigned int, bool);
					hi_score() {}

	unsigned int   level{0},
	               score{0};
	bool           win_game{false};

	bool           operator<(const hi_score& _other) const {

		return _other.score < score;
	}
};

class hi_score_manager {

	public:

	                          hi_score_manager();
	void                      submit(const hi_score&);
	const std::vector<hi_score>& get() const {return current_scores;}

	private:

	void                      save();
	void                      load();
	std::string               get_filename() const;

	std::vector<hi_score>     current_scores;
};
