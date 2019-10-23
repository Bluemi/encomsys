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

struct extra_component {
	int extra;

	extra_component() = default;
	extra_component(int i) : extra(i) {}
};

struct player_relation : encom::relation<player_name_t, position_t> {
	using encom::relation<player_name_t, position_t>::relation;
};

struct admin_player_relation : encom::relation<player_relation, extra_component> {
	using encom::relation<player_relation, extra_component>::relation;

	admin_player_relation() {}
};

using ensys = encom::encomsys<admin_player_relation, player_relation, player_name_t, position_t, extra_component>;

void print_player(const player_relation& player) {
	std::cout << "Player (name=" << player.get<player_name_t>().name << ", x=" << player.get<position_t>().x << ", y=" << player.get<position_t>().y << ")" << std::endl;
}

void print_admin(const admin_player_relation& admin) {
	std::cout << "Admin (name=" << admin.get<player_relation, player_name_t>().name <<
				 ", x=" << admin.get<player_relation, position_t>().x <<
				 ", y=" << admin.get<player_relation, position_t>().y <<
				 ", extra=" << admin.get<extra_component>().extra << ")" << std::endl;
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

	// add players
	encom::handle bjoern_handle = e.add(player_relation("bjoern", position_t(1.f, 2.f)));
	encom::handle heike_handle = e.add(player_relation("heike", position_t(11.f, 12.f)));

	// get player
	print_player(*e.get(bjoern_handle));
	print_player(*e.get(heike_handle));

	// add admin
	admin_player_relation admin_player(player_relation("admin", position_t(-1.f, -1.f)), extra_component(2));
	encom::handle admin_handle = e.add(admin_player);

	// get admin
	print_admin(*e.get(admin_handle));

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
