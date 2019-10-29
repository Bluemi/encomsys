#include <iostream>
#include <vector>


class Mob {
	public:
		void tick() {
			std::cout << "this mob is called" << get_string() << std::endl;
		}

		virtual std::string get_string() = 0;
};

class SubMob1 : public Mob {
	private:
		int id;
	public:
		SubMob1(int i) : id(i) {}

		virtual std::string get_string() {
			return "mob1 " + std::to_string(id);
		}
};

class SubMob2 : public Mob {
	private:
		float id;
		std::string name;
	public:
		SubMob2(float i, const std::string& s) : id(i), name(s) {}
		virtual std::string get_string() {
			return "mob2 " + std::to_string(id) + " with name: " +  name;
		}
};

int main() {
	std::vector<Mob*> mobs;
	mobs.push_back(new SubMob1(42));
	mobs.push_back(new SubMob2(42.f, "name"));

	for (Mob* m : mobs) {
		m->tick();
	}
}
