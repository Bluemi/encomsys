#ifndef __ENCOMSYS_CLASS__
#define __ENCOMSYS_CLASS__

#include <tuple>

#include "util/index_vector.hpp"

namespace encom {
	using ID_TYPE = uint64_t;

	template<typename T>
	using index_vector = __encom_internal::index_vector<T>;

	template<typename... ComponentTypes>
	class encomsys;

	template<typename T>
	struct ref {
		const ID_TYPE consecutive_index;
		const ID_TYPE array_index;

		ref(ID_TYPE consecutive_index, ID_TYPE array_index)
			: consecutive_index(consecutive_index), array_index(array_index)
		{ }
	};

	template<typename T>
	struct component_wrapper {
		ID_TYPE consecutive_index;
		T value;

		component_wrapper(ID_TYPE consecutive_index, const T& value)
			: consecutive_index(consecutive_index), value(value)
		{ }
	};

	template<typename... ComponentTypes>
	class encomsys {
		private:
			std::tuple<index_vector<component_wrapper<ComponentTypes>>...> _components;
			ID_TYPE _next_consecutive_id;

		public:
			explicit encomsys();

			/**
			 * Adds the given component into this encomsys.
			 *
			 * @param component The component to add to this encomsys
			 * @returns a ref to the added component
			 */
			template<typename T>
			ref<T> add(const T& component);

			/**
			 * @param ref The reference to the requests component
			 * @returns the component referenced by the given ref. If the component could not be found
			 * 			nullptr is returned.
			 */
			template<typename T>
			T* const get(const ref<T>&);

			/**
			 * @param ref The reference to the requests component
			 * @returns the component referenced by the given ref. If the component could not be found
			 * 			nullptr is returned.
			 */
			template<typename T>
			const T* const get(const ref<T>&) const;

			/**
			 * @param ref The reference to check
			 * @returns whether the given ref is present in this encomsys
			 */
			template<typename T>
			bool has_element(const ref<T>&) const;

			template<typename T>
			const index_vector<component_wrapper<T>>& get_components() const;

			template<typename T>
			index_vector<component_wrapper<T>>& get_components();

			/**
			 * Removes the component given by ref
			 *
			 * @param ref The reference pointing to the component, that should be removed
			 * @returns true, if the remove was successful, false otherwise
			 */
			template<typename T>
			bool remove(const ref<T>&);

			/**
			 * Executes func for every component of type T.
			 *
			 * @param func The function to execute for every component of type T
			 */
			template<typename T>
			void for_each(void (*func)(const T&));

			/**
			 * Executes func for every component of type T.
			 *
			 * @param func The function to execute for every component of type T
			 */
			template<typename T>
			void for_each(void (*func)(T&));

			/**
			 * Executes func for every component of type T.
			 *
			 * @param func The function to execute for every component of type T
			 */
			template<typename T>
			void for_each(void (*func)(T&, encomsys&));

			/**
			 * Executes func for every component of type T.
			 *
			 * @param func The function to execute for every component of type T
			 */
			template<typename T>
			void for_each(void (*func)(T&, const encomsys& encomsys));

			/**
			 * Executes func for every component of type T.
			 *
			 * @param func The function to execute for every component of type T
			 */
			template<typename T>
			void for_each(void (*func)(const T&, encomsys& encomsys));

			/**
			 * Executes func for every component of type T.
			 *
			 * @param func The function to execute for every component of type T
			 */
			template<typename T>
			void for_each(void (*func)(const T&, const encomsys& encomsys)) const;
	};

	template<typename... ComponentTypes>
	encomsys<ComponentTypes...>::encomsys() : _next_consecutive_id(0) {}

	template<typename... ComponentTypes>
	template<typename T>
	ref<T> encomsys<ComponentTypes...>::add(const T& component) {
		const component_wrapper<T> w(_next_consecutive_id, component);
		const ID_TYPE array_index = get_components<T>().add(w);
		return ref<T>(_next_consecutive_id++, array_index);
	}

	template<typename... ComponentTypes>
	template<typename T>
	T* const encomsys<ComponentTypes...>::get(const ref<T>& r) {
		if (has_element(r)) {
			return &get_components<T>().get(r.array_index).value;
		}
		return nullptr;
	}

	template<typename... ComponentTypes>
	template<typename T>
	const T* const encomsys<ComponentTypes...>::get(const ref<T>& r) const {
		if (has_element(r)) {
			return &get_components<T>().get(r.array_index).value;
		}
		return nullptr;
	}

	template<typename... ComponentTypes>
	template<typename T>
	bool encomsys<ComponentTypes...>::has_element(const ref<T>& r) const {
		// first check, if the array element is present
		if (get_components<T>().has_index(r.array_index)) {
			// second check, if the consecutive indices match
			return get_components<T>().get(r.array_index).consecutive_index == r.consecutive_index;
		}
		return false;
	}

	template<typename... ComponentTypes>
	template<typename T>
	const index_vector<component_wrapper<T>>& encomsys<ComponentTypes...>::get_components() const {
		return std::get<index_vector<component_wrapper<T>>>(_components);
	}

	template<typename... ComponentTypes>
	template<typename T>
	index_vector<component_wrapper<T>>& encomsys<ComponentTypes...>::get_components() {
		return std::get<index_vector<component_wrapper<T>>>(_components);
	}

	template<typename... ComponentTypes>
	template<typename T>
	bool encomsys<ComponentTypes...>::remove(const ref<T>& r) {
		if (has_element(r)) {
			return get_components<T>().remove(r.array_index);
		}
		return false;
	}

	template<typename... ComponentTypes>
	template<typename T>
	void encomsys<ComponentTypes...>::for_each(void (*func)(const T&)) {
		for (const component_wrapper<T>& t : get_components<T>()) {
			func(t.value);
		}
	}

	template<typename... ComponentTypes>
	template<typename T>
	void encomsys<ComponentTypes...>::for_each(void (*func)(T&, const encomsys& encomsys)) {
		for (const component_wrapper<T>& t : get_components<T>()) {
			func(t.value, *this);
		}
	}

	template<typename... ComponentTypes>
	template<typename T>
	void encomsys<ComponentTypes...>::for_each(void (*func)(T&, encomsys& encomsys)) {
		for (const component_wrapper<T>& t : get_components<T>()) {
			func(t.value, *this);
		}
	}

	template<typename... ComponentTypes>
	template<typename T>
	void encomsys<ComponentTypes...>::for_each(void (*func)(T&)) {
		for (T& t : get_components<T>()) {
			func(t);
		}
	}
}


#endif
