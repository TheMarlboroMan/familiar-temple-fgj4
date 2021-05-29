#include "powers.h"

bool powers::can_be_activated() const {

	return focus==max_focus;
}

void powers::force_stop() {

	remaining_time=0.f;
}

bool powers::is_active() const {

	return remaining_time > 0.f;
}

void powers::add_focus(int _quantity) {

	if(is_active()) {

		return;
	}

	focus+=_quantity;
	if(focus > max_focus) {

		focus=max_focus;
	}
}

void powers::activate() {

	focus=0;

	switch(current) {

		case power_type::time:
			remaining_time=5.;
		break;
		case power_type::fire:
			remaining_time=10.;
		break;
		case power_type::ammo:
		case power_type::health:
			remaining_time=0.;
		break;
	}
}

void powers::step(float _delta) {

	if(remaining_time > 0.f) {

		remaining_time-=_delta;
		if(remaining_time < 0.f) {

			remaining_time=0.f;
		}

		return;
	}

	cycle_time+=_delta;
	if(cycle_time >= 10.f) {

		cycle_time-=10.f;
		cycle();
	}
}

void powers::cycle() {

	switch(current) {

		case power_type::time: current=power_type::fire; break;
		case power_type::fire: current=power_type::ammo; break;
		case power_type::ammo: current=power_type::health; break;
		case power_type::health: current=power_type::time; break;
	}
}

void powers::reset() {

	focus=0;
	cycle_time=0.f;
	remaining_time=0.f;
	current=power_type::time;
}
