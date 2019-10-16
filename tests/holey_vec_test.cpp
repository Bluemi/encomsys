#include <iostream>

#include <holey_vec.hpp>

void print_vec(__encom_internal::holey_vec<int>& vec) {
	for (size_t index = 0; index < vec._instances.size(); ++index) {
		bool is_hole = vec._holes.find(index) != vec._holes.end();
		if (is_hole) {
			std::cout << index << ": X" << std::endl;
		} else {
			std::cout << index << ": " << vec._instances[index] << std::endl;
		}
	}
}

int main() {
	__encom_internal::holey_vec<int> vec;

	vec.add(100);
	vec.add(101);
	vec.add(102);
	vec.add(103);
	vec.add(104);

	std::cout << "initial vec:" << std::endl;
	print_vec(vec);

	vec.remove(0);
	vec.remove(2);
	std::cout << "removed at index 0 and 2" << std::endl;

	std::cout << "vec:" << std::endl;
	print_vec(vec);

	std::cout << "added 105 at " << vec.add(105) << std::endl;
	print_vec(vec);

	std::cout << "added 106 at " << vec.add(106) << std::endl;
	print_vec(vec);

	vec.add(107);
	vec.add(108);
	std::cout << "added 107 and 108" << std::endl;
	print_vec(vec);
}
