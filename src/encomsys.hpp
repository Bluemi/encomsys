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
	class ref {
		public:
			const ID_TYPE _consecutive_index;
			const ID_TYPE _array_index;

			ref(ID_TYPE consecutive_index, ID_TYPE array_index) : _consecutive_index(consecutive_index), _array_index(array_index) {}
	};

	template<typename... ComponentTypes>
	class encomsys {
		private:
			std::tuple<index_vector<ComponentTypes>...> _components;
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

			template<typename T>
			const index_vector<T>& get_components() const {
				return std::get<__encom_internal::index_vector<T>>(_components);
			}

			template<typename T>
			index_vector<T>& get_components() {
				return std::get<__encom_internal::index_vector<T>>(_components);
			}

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
		const ID_TYPE array_index = get_components<T>().add(component);
		return ref<T>(_next_consecutive_id, array_index);
	}

	template<typename... ComponentTypes>
	template<typename T>
	T* const encomsys<ComponentTypes...>::get(const ref<T>& r) {
		if (get_components<T>().has_index(r._array_index)) {
			return &get_components<T>().get(r._array_index);
		}
		return nullptr;
	}

	template<typename... ComponentTypes>
	template<typename T>
	const T* const encomsys<ComponentTypes...>::get(const ref<T>& r) const {
		if (get_components<T>().has_index(r._array_index)) {
			return &get_components<T>().get(r._array_index);
		}
		return nullptr;
	}

	template<typename... ComponentTypes>
	template<typename T>
	bool encomsys<ComponentTypes...>::remove(const ref<T>& r) {
		return get_components<T>().remove(r._array_index);
	}

	template<typename... ComponentTypes>
	template<typename T>
	void encomsys<ComponentTypes...>::for_each(void (*func)(const T&)) {
		for (const T& t : get_components<T>()) {
			func(t);
		}
	}

	template<typename... ComponentTypes>
	template<typename T>
	void encomsys<ComponentTypes...>::for_each(void (*func)(T&, const encomsys& encomsys)) {
		for (const T& t : get_components<T>()) {
			func(t, *this);
		}
	}

	template<typename... ComponentTypes>
	template<typename T>
	void encomsys<ComponentTypes...>::for_each(void (*func)(T&, encomsys& encomsys)) {
		for (const T& t : get_components<T>()) {
			func(t, *this);
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
