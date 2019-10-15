#ifndef __ENCOMSYS_CLASS__
#define __ENCOMSYS_CLASS__

#include <tuple>
#include <vector>

namespace encom {
	using id = unsigned long;

	template<typename... ComponentTypes>
	class encomsys {
		private:
			std::tuple<std::vector<ComponentTypes>...> _components;
			int _next_id;

		public:
			explicit encomsys();

			template<typename T>
			id add_component(const T& component);

			template<typename T>
			void for_each(void (*func)(const T&));

			template<typename T>
			void for_each(void (*func)(T&));
	};

	template<typename... ComponentTypes>
	encomsys<ComponentTypes...>::encomsys() {}

	template<typename... ComponentTypes>
	template<typename T>
	id encomsys<ComponentTypes...>::add_component(const T& component) {
		std::get<std::vector<T>>(_components).push_back(component);
		return std::get<std::vector<T>>(_components).size();
	}

	template<typename... ComponentTypes>
	template<typename T>
	void encomsys<ComponentTypes...>::for_each(void (*func)(const T&)) {
		for (const T& t : std::get<std::vector<T>>(_components)) {
			func(t);
		}
	}

	template<typename... ComponentTypes>
	template<typename T>
	void encomsys<ComponentTypes...>::for_each(void (*func)(T&)) {
		for (T& t : std::get<std::vector<T>>(_components)) {
			func(t);
		}
	}
}


#endif
