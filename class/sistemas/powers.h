#pragma once

class powers {

	public:

	enum class power_type {
		time, fire, ammo, health
	};

	bool               can_be_activated() const;
	bool               is_active() const;

	int                get_focus() const {return focus;}
	void               add_focus(int);
	void               activate();
	void               step(float);
	void               reset();
	void               force_stop();
	power_type         get_current() const {return current;}

	private:

	void               cycle();

	int                focus{0};
	static const int   max_focus{50};

	float              cycle_time{0.f},
	                   remaining_time{0.f};

	power_type         current{power_type::time};
};
