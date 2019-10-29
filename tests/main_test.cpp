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
	position_t(const float x, const float y) : x(x), y(y) {}

	float x;
	float y;
};

struct password_component {
	std::string passwd;

	password_component() = default;
	password_component(const std::string& pw) : passwd(pw) {}
};

struct player_relation : encom::relation<player_name_t, position_t> {
	using encom::relation<player_name_t, position_t>::relation;
};

struct admin_player_relation : encom::relation<player_relation, password_component> {
	using encom::relation<player_relation, password_component>::relation;

	admin_player_relation() {}
};

using ensys = encom::encomsys<admin_player_relation, player_relation, player_name_t, position_t, password_component>;

void print_position(const position_t& pos) {
	std::cout << "Position (x=" << pos.x << ", y=" << pos.y << ")" << std::endl;
}

void print_player(const player_relation& player) {
	std::cout << "Player (name=" << player.get<player_name_t>().name << ", x=" << player.get<position_t>().x << ", y=" << player.get<position_t>().y << ")" << std::endl;
}

void print_admin(const admin_player_relation& admin) {
	std::cout << "Admin (name=" << admin.get<player_relation, player_name_t>().name <<
				 ", x=" << admin.get<player_relation, position_t>().x <<
				 ", y=" << admin.get<player_relation, position_t>().y <<
				 ", pw=" << admin.get<password_component>().passwd << ")" << std::endl;
}

void do_physics(const player_relation::as_ref& player) {
	std::get<position_t&>(player).x += 1.f;
	std::get<position_t&>(player).y += 1.f;
}

int main() {
	ensys ensys;

	// TEST add ------------------------------------------------------
	// add position
	encom::handle pos_handle = ensys.add(position_t(42.f, 43.f));

	// add player
	encom::handle player_handle = ensys.add(player_relation("player", position_t(1.f, 2.f)));

	// add admin
	encom::handle admin_handle = ensys.add(admin_player_relation(player_relation("admin", position_t(-1.f, -1.f)), password_component("pw")));


	// TEST get ------------------------------------------------------
	// get position
	print_position(*ensys.get(pos_handle));

	// get player
	print_player(*ensys.get(player_handle));

	// get admin
	print_admin(*ensys.get(admin_handle));

	// TEST get_ref ------------------------------------------------------
	// get position
	print_position(*ensys.get_ref(pos_handle));

	// get player
	do_physics(*ensys.get_ref(player_handle));

	print_player(*ensys.get(player_handle));

	/*
	std::cout << "initial players:" << std::endl;
	ensys.for_each(print_player);
	std::cout << std::endl;
	*/

	/*
	std::cout << "removed ";
	// print_player(*ensys.get(player_ref));
	ensys.remove(player_ref);

	std::cout << "remove success: " << ensys.remove(player_ref) << std::endl;

	std::cout << "after remove" << std::endl;
	ensys.for_each(print_player);
	std::cout << std::endl;

	ensys.add(player_relation("daniel", position_t(3.4f, 123.f)));
	ensys.for_each(print_player);
	std::cout << std::endl;
	*/

	/*
	const player_relation* const p = ensys.get(player_ref);
	if (p) {
		print_player(*p);
	} else {
		std::cout << "player ist nicht mehr im encosys" << std::endl;
	}
	*/
}
