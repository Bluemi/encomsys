#include <iostream>
#include <string>

#include "encomsys.hpp"

struct player_name_t {
	player_name_t() = default;
	player_name_t(const std::string& name) : name(name) {}

	std::string name;

};

struct position_t {
	position_t() = default;
	position_t(const float x, const float y) : x(x), y(y), v_x(0.f), v_y(0.f) {}

	float x;
	float y;

	float v_x;
	float v_y;
};

struct player_relation : encom::relation<player_name_t, position_t> {
	using encom::relation<player_name_t, position_t>::relation;
};

using ensys = encom::encomsys<player_relation, player_name_t, position_t>;

void print_player(const player_relation& player) {
	std::cout << "Player (name=" << std::get<player_name_t>(player).name << ", x=" << std::get<position_t>(player).x << ", y=" << std::get<position_t>(player).y << ")" << std::endl;
}

/*
void do_physics(player_relation::as_ref& player) {
	std::get<position_t&>(player).x += std::get<position_t&>(player).v_x;
	std::get<position_t&>(player).y += std::get<position_t&>(player).v_y;

	std::get<position_t&>(player).v_y -= 0.1f; // gravity
}
*/

int main() {
	ensys e;

	player_relation bjoern("bjoern", position_t(1.f, 2.f));
	encom::handle bjoern_handle = e.add(bjoern);
	e.add(player_relation("heike", position_t(11.f, 12.f)));

	std::cout << "0.x: " << e.get_components<position_t>().get(0).get_value().x << std::endl;

	print_player(*e.get(bjoern_handle));

	/*
	std::cout << "initial players:" << std::endl;
	e.for_each(print_player);
	std::cout << std::endl;
	*/

	/*
	std::cout << "removed ";
	// print_player(*e.get(bjoern_ref));
	e.remove(bjoern_ref);

	std::cout << "remove success: " << e.remove(bjoern_ref) << std::endl;

	std::cout << "after remove" << std::endl;
	e.for_each(print_player);
	std::cout << std::endl;

	e.add(player_relation("daniel", position_t(3.4f, 123.f)));
	e.for_each(print_player);
	std::cout << std::endl;
	*/

	/*
	const player_relation* const p = e.get(bjoern_ref);
	if (p) {
		print_player(*p);
	} else {
		std::cout << "bjoern ist nicht mehr im encosys" << std::endl;
	}
	*/
}
