#include <iostream>
#include <string>

#include "encomsys.hpp"

struct Player {
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

	player.v_y -= 0.1f; // gravity
}

int main() {
	ensys e;

	encom::ref<Player> bjoern = e.add(Player("björn", 1.0f, 2.3f));
	e.add(Player("heike", -2.0f, 3.1f));

	std::cout << "initial players:" << std::endl;
	e.for_each(print_player);
	std::cout << std::endl;

	std::cout << "removed ";
	print_player(*e.get(bjoern));
	e.remove(bjoern);

	std::cout << "remove success: " << e.remove(bjoern) << std::endl;

	std::cout << "after remove" << std::endl;
	e.for_each(print_player);
	std::cout << std::endl;

	e.add(Player("daniel", 3.4f, 123.f));
	e.for_each(print_player);
	std::cout << std::endl;

	print_player(*e.get(bjoern));
	std::cout << std::endl;
}
