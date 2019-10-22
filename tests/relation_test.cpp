#include <iostream>

#include <relation.hpp>

struct my_component1 {
	int i = 0;
};

struct my_component2 {
	int i = 1;
};

struct my_relation : encom::relation<my_component1, my_component2> {};
struct my_relation2 : encom::relation<int, float> {};
struct no_relation {};

int main() {
	std::cout << encom::is_relation<my_relation>::value << std::endl;
	std::cout << encom::is_relation<my_relation2>::value << std::endl;
	std::cout << encom::is_relation<no_relation>::value << std::endl;
	return 0;
}
