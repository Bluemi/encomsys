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

template<typename ...Ts>
struct relation_t {
	public:
		relation_t(Ts... ts) : _component_refs(ts...) {}

		std::tuple<encom::ref<Ts>...> _component_refs;

		using instance = std::tuple<std::optional<Ts* const>...>;
};

struct enemy_relation_t : relation_t<position_t, player_name_t> {};
struct player_relation_t : relation_t<position_t, player_name_t> {};

using ensys = encom::encomsys<player_relation_t, player_name_t, position_t>;

void print_player(const player_relation_t::instance& player) {
	std::cout << "Player (name=" << std::get<player_name_t>(player).name << ", x=" << std::get<position_t>(player).x << ", y=" << std::get<position_t>(player).y << ")" << std::endl;
}

void do_physics(player_relation_t::instance& player) {
	player.position.x += player.position.v_x;
	player.position.y += player.position.v_y;

	player.position.v_y -= 0.1f; // gravity
}

int main() {
	ensys e;

	encom::ref<player_relation_t> bjoern = e.add(player_relation_t(e.add(player_name_t("bjoern")), e.add(position_t(1.f, 2.f))));
	encom::ref<player_relation_t> heike = e.add(player_relation_t(e.add(player_name_t("heike")), e.add(position_t(11.f, 12.f))));

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

	const Player* const p = e.get(bjoern);
	if (p) {
		print_player(*p);
	} else {
		std::cout << "bjoern ist nicht mehr im encosys" << std::endl;
	}
}
