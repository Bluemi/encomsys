#include <iostream>
#include <string>

#include "encomsys.hpp"

class Player {
	public:
		Player(const std::string& name, float x, float y) : name(name), x(x), y(y), v_x(0.f), v_y(0.f) {}

		std::string name;

		float x;
		float y;

		float v_x;
		float v_y;
};

using ensys = encom::encomsys<Player>;

void print_player(const Player& player) {
	std::cout << "Player (name=" << player.name << ", x=" << player.x << ", y=" << player.y << ")" << std::endl;
}

void do_physics(Player& player) {
	player.x += player.v_x;
	player.y += player.v_y;

	player.v_y -= 1.f; // gravity
}

int main() {
	ensys e;

	e.add_component(Player("björn", 1.0f, 2.3f));
	e.add_component(Player("heike", -2.0f, 3.1f));

	e.for_each(print_player);
	for (unsigned int i = 0; i < 3; i++) {
		e.for_each(do_physics);
	}

	e.for_each(print_player);
}
