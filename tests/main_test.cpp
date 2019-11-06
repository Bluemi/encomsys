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
	position_t(const float x) : x(x) {}

	float x;
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
	std::cout << "Position (x=" << pos.x << ")" << std::endl;
}

void print_player(const player_relation& player) {
	std::cout << "Player (name=" << player.get<player_name_t>().name << ", x=" << player.get<position_t>().x << ")" << std::endl;
}

void print_admin(const admin_player_relation& admin) {
	std::cout << "Admin (name=" << admin.get<player_relation, player_name_t>().name <<
				 ", x=" << admin.get<player_relation, position_t>().x <<
				 ", pw=" << admin.get<password_component>().passwd << ")" << std::endl;
}

void do_physics(const player_relation::as_ref& player) {
	std::get<position_t&>(player).x += 1.f;
}

void change_password(admin_player_relation::as_ref admin) {
	admin.get<password_component>().passwd = "new password";
}

void change_admin_position(const admin_player_relation::as_ref& admin) {
	admin.get<player_relation, position_t>().x = 100;
}

int main() {
	ensys ensys;

	// TEST add ------------------------------------------------------
	// add position
	encom::handle pos_handle = ensys.add(position_t(0.f));

	// add player
	encom::handle player_handle = ensys.add(player_relation("player", position_t(0.f)));

	// add admin
	encom::handle admin_handle = ensys.add(admin_player_relation(player_relation("admin", position_t(0.f)), password_component("pw")));


	// TEST get ------------------------------------------------------
	// get position
	print_position(*ensys.get(pos_handle));

	// get player
	print_player(*ensys.get(player_handle));

	// get admin
	print_admin(*ensys.get(admin_handle));

	// TEST get_ref ------------------------------------------------------
	std::cout << std::endl << "Changing player and admin" << std::endl;
	// get position
	print_position(*ensys.get_ref(pos_handle));

	// get player
	do_physics(*ensys.get_ref(player_handle));

	print_player(*ensys.get(player_handle));

	change_password(*ensys.get_ref(admin_handle));
	change_admin_position(*ensys.get_ref(admin_handle));

	print_admin(*ensys.get(admin_handle));

	// TEST remove ------------------------------------------------------
	std::cout << std::endl << "Testing remove" << std::endl;
	std::cout << "number of positions: " << ensys.get_components<position_t>().size() << std::endl;

	ensys.remove(player_handle);

	std::cout << "number of positions after player got removed: " << ensys.get_components<position_t>().size() << std::endl;
}
