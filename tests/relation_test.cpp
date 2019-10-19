#include <iostream>

#include <relation.hpp>

struct my_relation : encom::relation<int, float> {};
struct no_relation {};

int main() {
	std::cout << encom::is_relation<my_relation>::value << std::endl;
	std::cout << encom::is_relation<no_relation>::value << std::endl;
	return 0;
}
