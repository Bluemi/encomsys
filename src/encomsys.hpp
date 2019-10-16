#ifndef __ENCOMSYS_CLASS__
#define __ENCOMSYS_CLASS__

#include <tuple>

// #include "holey_vec.hpp"
#include <vector>

namespace encom {
	using ID_TYPE = uint64_t;

	template<typename... ComponentTypes>
	class encomsys;

	template<typename T>
	class ref {
		private:
			const ID_TYPE _consecutive_index;
			const ID_TYPE _array_index;
		public:
			ref(ID_TYPE consecutive_index, ID_TYPE array_index) : _consecutive_index(consecutive_index), _array_index(array_index) {}

			template<typename... ComponentTypes>
			const T& get(const encomsys<ComponentTypes...>& encomsys) const;
	};

	template<typename... ComponentTypes>
	class encomsys {
		private:
			std::tuple<std::vector<ComponentTypes>...> _components;
			ID_TYPE _next_consecutive_id;

		public:
			explicit encomsys();

			template<typename T>
			ref<T> add_component(const T& component);

			template<typename T>
			void for_each(void (*func)(const T&));

			template<typename T>
			void for_each(void (*func)(T&));
	};

	template<typename... ComponentTypes>
	encomsys<ComponentTypes...>::encomsys() : _next_consecutive_id(0) {}

	template<typename... ComponentTypes>
	template<typename T>
	ref<T> encomsys<ComponentTypes...>::add_component(const T& component) {
		/*
		const ID_TYPE array_index = std::get<std::vector<T>>(_components).add(component);
		*/
		return ref<T>(_next_consecutive_id, 0);  // TODO
	}

	template<typename... ComponentTypes>
	template<typename T>
	void encomsys<ComponentTypes...>::for_each(void (*func)(const T&)) {
		/*
		for (
			__encom_internal::holey_vec_iterator<T> iter = std::get<__encom_internal::holey_vec<T>>(_components).begin();
			iter != std::get<__encom_internal::holey_vec<T>>(_components).end();
			++iter
		) {
			func(*iter);
		}
		*/
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
