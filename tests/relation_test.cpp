#include <iostream>
#include <functional>

#include <relation.hpp>

struct C1 {
	int i = 0;
};

struct C2 {
	int i = 1;
};

struct A : encom::relation<C1, C2> {};
struct B : encom::relation<int, float> {};
struct Relation : encom::relation<A, B> {};
struct NoRelation {};

void test_is_relation() {
	std::cout << "is relation:" << std::endl;
	std::cout << "A: " << encom::is_relation_v<A> << std::endl;
	std::cout << "B: " << encom::is_relation_v<B> << std::endl;
	std::cout << "Relation: " << encom::is_relation_v<Relation> << std::endl;
	std::cout << "NoRelation: " << encom::is_relation_v<NoRelation> << std::endl;
}

void test_as_ref_get() {
	C1 c1;
	C2 c2;
	A::as_ref a_ref = std::make_tuple(std::ref(c1), std::ref(c2));
	std::cout << a_ref.get<C1>().i << std::endl;
}

void test_as_ref_get_rec() {
	C1 c1;
	C2 c2;

	int i = 1;
	float f = 2.f;

	A::as_ref a_ref = std::make_tuple(std::ref(c1), std::ref(c2));
	B::as_ref b_ref = std::make_tuple(std::ref(i), std::ref(f));
	Relation::as_ref r_ref = std::make_tuple(a_ref, b_ref);
	std::cout << r_ref.get<A, C1>().i << std::endl;
}

int main() {
	test_as_ref_get();
	test_as_ref_get_rec();
	return 0;
}
