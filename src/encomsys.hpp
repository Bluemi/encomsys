#ifndef __ENCOMSYS_CLASS__
#define __ENCOMSYS_CLASS__

// #define LOG_PRINTS

#include <tuple>
#include <functional>
#ifdef LOG_PRINTS
#include <iostream>
#endif

#include "util/index_vector.hpp"
#include "util/types.hpp"
#include "handle.hpp"
#include "relation.hpp"

namespace encom {
	template<typename ...ComponentTypes>
	class encomsys;

	template<typename ComponentType, typename __Specialization=void>
	struct component_wrapper {
		ID_TYPE consecutive_index;
		std::uint32_t number_of_references;
		ComponentType value;

		component_wrapper(ID_TYPE consecutive_index, std::uint32_t number_of_references, const ComponentType& value)
			: consecutive_index(consecutive_index), number_of_references(number_of_references), value(value)
		{ }

		ComponentType& get_value() {
			#ifdef LOG_PRINTS
			std::cout << "component_wrapper<" << typeid(ComponentType).name() << ">::get_value()" << std::endl;
			#endif
			return value;
		}

		const ComponentType& get_value() const {
			#ifdef LOG_PRINTS
			std::cout << "component_wrapper<" << typeid(ComponentType).name() << ">::get_value() const" << std::endl;
			#endif
			return value;
		}

		ComponentType& get_ref() {
			#ifdef LOG_PRINTS
			std::cout << "component_wrapper<" << typeid(ComponentType).name() << ">::get_ref()" << std::endl;
			#endif
			return value;
		}

		const ComponentType& get_ref() const {
			#ifdef LOG_PRINTS
			std::cout << "component_wrapper<" << typeid(ComponentType).name() << ">::get_ref() const" << std::endl;
			#endif
			return value;
		}

		template<typename ...ComponentTypes>
		inline void remove_childs(encomsys<ComponentTypes...>*) {}
	};

	/**
	 * This specialization contains the relations sub type __component_handles, to point to
	 * the components locations.
	 */
	template<typename RelationType>
	struct component_wrapper<RelationType, std::enable_if_t<is_relation_v<RelationType>>> {
		ID_TYPE consecutive_index;
		std::uint32_t number_of_references;
		typename RelationType::__component_handles _handles;

		component_wrapper(ID_TYPE consecutive_index, std::uint32_t number_of_references, const typename RelationType::__component_handles& handles)
			: consecutive_index(consecutive_index), number_of_references(number_of_references), _handles(handles)
		{}

		/**
		 * Helper function for get_value()
		 * The template argument I counts from 0 to sizeof...(RelationComponentTypes)-1.
		 * Every call to relation_get_helper() expects an relation argument.
		 *   - The relation should contain a component of the I-th type of RelationComponentTypes...
		 */
		template<size_t I = 0, typename ...RelationComponentTypes, typename ...ComponentTypes>
		std::enable_if_t<I == sizeof...(RelationComponentTypes)>
		relation_get_helper(
			std::tuple<RelationComponentTypes...>*,
			const encomsys<ComponentTypes...>* const
		) const { }

		template<size_t I = 0, typename ...RelationComponentTypes, typename ...ComponentTypes>
		std::enable_if_t<I < sizeof...(RelationComponentTypes)>
		relation_get_helper(
			std::tuple<RelationComponentTypes...>* relation,
			const encomsys<ComponentTypes...>* const encomsys
		) const {
			#ifdef LOG_PRINTS
			std::cout << "relation_get_helper<I=" << I << ", sizeof...(RelationComponentTypes)=" << sizeof...(RelationComponentTypes) << ">()" << std::endl;
			#endif
			using current_relation_component_type = std::tuple_element_t<I, std::tuple<RelationComponentTypes...>>;
			std::get<I>(*relation) = *encomsys->get(std::get<I>(_handles));
			relation_get_helper<I+1>(relation, encomsys);
		}

		/**
		 * Returns the relation instance as object, without references.
		 * @param encomsys The encomsys to retrieve the components from
		 */
		template<typename ...ComponentTypes>
		RelationType get_value(const encomsys<ComponentTypes...>* const encomsys) const {
			#ifdef LOG_PRINTS
			std::cout << "component_wrapper<" << typeid(RelationType).name() << ">::get_value()" << std::endl;
			#endif
			RelationType relation;
			relation_get_helper(&relation, encomsys);
			return relation;
		}

		// handle_to_ref for relations
		template<typename RelationComponentType, typename ...EncomComponentTypes>
		std::enable_if_t<
			is_relation_v<RelationComponentType>,
			typename RelationComponentType::as_ref
		> handle_to_ref(
			const handle<RelationComponentType>& handle,
			encomsys<EncomComponentTypes...>* const encomsys
		) const {
			return *encomsys->get_ref(handle);
		}

		// handle_to_ref for components
		template<typename RelationComponentType, typename ...EncomComponentTypes>
		std::enable_if_t<
			!is_relation_v<RelationComponentType>,
			typename std::reference_wrapper<RelationComponentType>
		> handle_to_ref(
			const handle<RelationComponentType>& handle,
			encomsys<EncomComponentTypes...>* const encomsys
		) const {
			return std::ref(*encomsys->get_ref(handle));
		}

		template<size_t ...I, typename ...RelationComponentTypes, typename ...EncomComponentTypes>
		typename RelationType::as_ref get_ref_helper_impl(
			[[maybe_unused]] std::index_sequence<I...>,
			const std::tuple<handle<RelationComponentTypes>...>& handles,
			encomsys<EncomComponentTypes...>* const encomsys
		) const {
			return typename RelationType::as_ref(std::make_tuple(handle_to_ref(std::get<I>(handles), encomsys) ...));
		}

		template<typename ...RelationComponentTypes, typename ...EncomComponentTypes>
		typename RelationType::as_ref get_ref_helper_expand_relation(
			const std::tuple<handle<RelationComponentTypes>...>& handles,
			encomsys<EncomComponentTypes...>* const encomsys
		) const {
			constexpr size_t tuple_size = std::tuple_size<std::tuple<RelationComponentTypes...>>::value;
			return get_ref_helper_impl(std::make_index_sequence<tuple_size>(), handles, encomsys);
		}

		/**
		 * Returns a relation as their reference type (relation::as_ref).
		 * @param encomsys The encomsys to retrieve the components of this relation
		 */
		template<typename ...ComponentTypes>
		typename RelationType::as_ref get_ref(encomsys<ComponentTypes...>* const encomsys) const {
			return get_ref_helper_expand_relation(_handles, encomsys);
		}

		template<size_t I = 0, typename ...RelationComponentTypes, typename ...ComponentTypes>
		std::enable_if_t<I == sizeof...(RelationComponentTypes)>
		remove_childs_impl(
			const std::tuple<handle<RelationComponentTypes>...>&,
			encomsys<ComponentTypes...>* const
		) {}

		template<size_t I = 0, typename ...RelationComponentTypes, typename ...ComponentTypes>
		std::enable_if_t<I < sizeof...(RelationComponentTypes)>
		remove_childs_impl(
			const std::tuple<handle<RelationComponentTypes>...>& relation_handles,
			encomsys<ComponentTypes...>* const encomsys
		) {
			encomsys->__decrease_number_of_references(std::get<I>(relation_handles));
			encomsys->remove(std::get<I>(relation_handles));
			remove_childs_impl<I+1>(relation_handles, encomsys);
		}

		/**
		 * Recursively removes child components of the wrapped relation instance
		 *
		 * @param encomsys The entity component system from which to remove the childs
		 */
		template<typename ...ComponentTypes>
		void remove_childs(encomsys<ComponentTypes...>* encomsys) {
			remove_childs_impl(_handles, encomsys);
		}
	};

	template<typename... ComponentTypes>
	class encomsys {
		private:
			std::tuple<index_vector<component_wrapper<ComponentTypes>>...> _components;
			ID_TYPE _next_consecutive_id;

		public:
			explicit encomsys();

			template<typename ComponentType>
			void __decrease_number_of_references(const handle<ComponentType>& handle);

			/**
			 * Adds the given component or relation into this encomsys.
			 * It is assumed that the given component or relation is not references by other relations.
			 *
			 * @param component The component to add to this encomsys
			 * @returns a handle to the added component
			 */
			template<typename ComponentType>
			handle<ComponentType> add(const ComponentType& component);

			/**
			 * Adds the given component into this encomsys.
			 *
			 * @param component The component to add to this encomsys
			 * @param number_of_references The number of relations referencing this component
			 * @returns a handle to the added component
			 */
			template<typename ComponentType>
			std::enable_if_t<!is_relation_v<ComponentType>, handle<ComponentType>> add(const ComponentType& component, std::uint32_t number_of_references);

			/**
			 * Adds the given relation into this encomsys.
			 *
			 * @param relation_component The relation component to add to this encomsys
			 * @param number_of_references The number of relations referencing this relation
			 * @returns a handle to the added relation
			 */
			template<typename RelationType>
			std::enable_if_t<is_relation_v<RelationType>, handle<RelationType>> add(const RelationType& relation_component, std::uint32_t number_of_references);

			/**
			 * @param handle The handle to the requested component
			 * @returns the component referenced by the given handle. If the component could not be found
			 * 			nullptr is returned.
			 */
			template<typename ComponentType>
			std::enable_if_t<!is_relation_v<ComponentType>, std::optional<ComponentType>>
			get(const handle<ComponentType>&) const;

			/**
			 * @param handle The handle to the requests component
			 * @returns the component referenced by the given handle. If the component could not be found
			 * 			nullptr is returned.
			 */
			template<typename RelationType>
			std::enable_if_t<is_relation_v<RelationType>, std::optional<RelationType>>
			get(const handle<RelationType>&) const;

			/**
			 * @param handle The handle to the requested component
			 */
			template<typename ComponentType>
			std::enable_if_t<!is_relation_v<ComponentType>, ComponentType* const>
			get_ref(const handle<ComponentType>& handle);

			/**
			 * @param handle The handle to the requested component
			 */
			template<typename RelationType>
			std::enable_if_t<is_relation_v<RelationType>, std::optional<typename RelationType::as_ref>>
			get_ref(const handle<RelationType>& handle);

			/**
			 * @param handle The reference to check
			 * @returns whether the given handle is present in this encomsys
			 */
			template<typename ComponentType>
			bool has_element(const handle<ComponentType>&) const;

			template<typename ComponentType>
			const index_vector<component_wrapper<ComponentType>>& get_components() const;

			template<typename ComponentType>
			index_vector<component_wrapper<ComponentType>>& get_components();

			/**
			 * Removes the component given by handle
			 *
			 * @param handle The reference pointing to the component, that should be removed
			 * @returns true, if the remove was successful, false otherwise
			 */
			template<typename ComponentType>
			bool remove(const handle<ComponentType>&);

			/**
			 * Executes func for every component of type <ComponentType>.
			 *
			 * @param func The function to execute for every component of type <ComponentType>
			 */
			template<typename ComponentType>
			void for_each(void (*func)(const ComponentType&));

			/**
			 * Executes func for every component of type <ComponentType>.
			 *
			 * @param func The function to execute for every component of type <ComponentType>
			 */
			template<typename ComponentType>
			void for_each(void (*func)(ComponentType&));

			/**
			 * Executes func for every component of type <ComponentType>.
			 *
			 * @param func The function to execute for every component of type <ComponentType>
			 */
			template<typename ComponentType>
			void for_each(void (*func)(ComponentType&, encomsys&));

			/**
			 * Executes func for every component of type <ComponentType>.
			 *
			 * @param func The function to execute for every component of type <ComponentType>
			 */
			template<typename ComponentType>
			void for_each(void (*func)(ComponentType&, const encomsys& encomsys));

			/**
			 * Executes func for every component of type <ComponentType>.
			 *
			 * @param func The function to execute for every component of type <ComponentType>
			 */
			template<typename ComponentType>
			void for_each(void (*func)(const ComponentType&, encomsys& encomsys));

			/**
			 * Executes func for every component of type <ComponentType>.
			 *
			 * @param func The function to execute for every component of type <ComponentType>
			 */
			template<typename ComponentType>
			void for_each(void (*func)(const ComponentType&, const encomsys& encomsys)) const;
	};

	template<typename... ComponentTypes>
	encomsys<ComponentTypes...>::encomsys() : _next_consecutive_id(0) {}

	template<typename... ComponentTypes>
	template<typename ComponentType>
	void encomsys<ComponentTypes...>::__decrease_number_of_references(const handle<ComponentType>& handle) {
		if (has_element(handle)) {
			get_components<ComponentType>().get(handle.array_index).number_of_references--;
		}
	}

	template<typename... ComponentTypes>
	template<typename ComponentType>
	std::enable_if_t<!is_relation_v<ComponentType>, handle<ComponentType>> encomsys<ComponentTypes...>::add(const ComponentType& component, std::uint32_t number_of_references) {
		const component_wrapper<ComponentType> w(_next_consecutive_id, number_of_references, component);
		const ID_TYPE array_index = get_components<ComponentType>().add(w);
		return handle<ComponentType>(_next_consecutive_id++, array_index);
	}

	template<std::size_t I = 0, typename ...RelationComponentTypes, typename ...ComponentTypes>
	std::enable_if_t<I == sizeof...(RelationComponentTypes)>
	add_relation_components(
		const std::tuple<RelationComponentTypes...>&,
		std::tuple<handle<RelationComponentTypes>...>*,
		encomsys<ComponentTypes...>*
	) {}

	template<std::size_t I = 0, typename ...RelationComponentTypes, typename ...ComponentTypes>
	std::enable_if_t<I < sizeof...(RelationComponentTypes)>
	add_relation_components(
		const std::tuple<RelationComponentTypes...>& relation_components,
		std::tuple<handle<RelationComponentTypes>...>* handles,
		encomsys<ComponentTypes...>* encomsys
	) {
		using component_type = std::tuple_element_t<I, std::tuple<RelationComponentTypes...>>;
		handle<component_type> component_handle = encomsys->add(std::get<I>(relation_components), 1);
		add_relation_components<I+1>(relation_components, handles, encomsys);
		std::get<handle<component_type>>(*handles) = component_handle;
	}

	template<typename ...RelationComponentTypes, typename ...ComponentTypes>
	std::tuple<handle<RelationComponentTypes>...> relation_add_helper(const std::tuple<RelationComponentTypes...>& relation_components, encomsys<ComponentTypes...>* encomsys) {
		std::tuple<handle<RelationComponentTypes>...> handles;
		add_relation_components(relation_components, &handles, encomsys);
		return handles;
	}

	template<typename... ComponentTypes>
	template<typename ComponentType>
	handle<ComponentType> encomsys<ComponentTypes...>::add(const ComponentType& component) {
		return add(component, 0);
	}

	template<typename... ComponentTypes>
	template<typename RelationType>
	std::enable_if_t<is_relation_v<RelationType>, handle<RelationType>> encomsys<ComponentTypes...>::add(const RelationType& relation_component, std::uint32_t number_of_references) {
		typename RelationType::__component_handles handles = relation_add_helper(relation_component, this);

		component_wrapper<RelationType> w(_next_consecutive_id, number_of_references, handles);

		ID_TYPE array_index = get_components<RelationType>().add(w);

		return handle<RelationType>(_next_consecutive_id++, array_index);
	}

	template<typename... ComponentTypes>
	template<typename ComponentType>
	std::enable_if_t<!is_relation_v<ComponentType>, std::optional<ComponentType>>
	encomsys<ComponentTypes...>::get(const handle<ComponentType>& component_handle) const {
		if (has_element(component_handle)) {
			return get_components<ComponentType>().get(component_handle.array_index).get_value();
		}
		return {};
	}

	template<typename... ComponentTypes>
	template<typename RelationType>
	std::enable_if_t<
		is_relation_v<RelationType>,
		std::optional<RelationType>
	>
	encomsys<ComponentTypes...>::get(const handle<RelationType>& relation_handle) const {
		if (has_element(relation_handle)) {
			return std::optional(get_components<RelationType>().get(relation_handle.array_index).get_value(this));
		}
		return {};
	}

	template<typename ...ComponentTypes>
	template<typename ComponentType>
	std::enable_if_t<!is_relation_v<ComponentType>, ComponentType* const>
	encomsys<ComponentTypes...>::get_ref(const handle<ComponentType>& component_handle) {
		if (has_element(component_handle)) {
			return &get_components<ComponentType>().get(component_handle.array_index).get_ref();
		}
		return nullptr;
	}

	template<typename ...ComponentTypes>
	template<typename RelationType>
	std::enable_if_t<is_relation_v<RelationType>, std::optional<typename RelationType::as_ref>>
	encomsys<ComponentTypes...>::get_ref(const handle<RelationType>& component_handle) {
		if (has_element(component_handle)) {
			return std::optional(get_components<RelationType>().get(component_handle.array_index).get_ref(this));
		}
		return {};
	}

	template<typename... ComponentTypes>
	template<typename ComponentType>
	bool encomsys<ComponentTypes...>::has_element(const handle<ComponentType>& r) const {
		// first check, if the array element is present
		if (get_components<ComponentType>().has_index(r.array_index)) {
			// second check, if the consecutive indices match
			return get_components<ComponentType>().get(r.array_index).consecutive_index == r.consecutive_index;
		}
		return false;
	}

	template<typename... ComponentTypes>
	template<typename ComponentType>
	const index_vector<component_wrapper<ComponentType>>& encomsys<ComponentTypes...>::get_components() const {
		return std::get<index_vector<component_wrapper<ComponentType>>>(_components);
	}

	template<typename... ComponentTypes>
	template<typename ComponentType>
	index_vector<component_wrapper<ComponentType>>& encomsys<ComponentTypes...>::get_components() {
		return std::get<index_vector<component_wrapper<ComponentType>>>(_components);
	}

	template<typename... ComponentTypes>
	template<typename ComponentType>
	bool encomsys<ComponentTypes...>::remove(const handle<ComponentType>& h) {
		if (has_element(h)) {
			component_wrapper<ComponentType> w = get_components<ComponentType>().get(h.array_index);

			if (w.number_of_references == 0) {
				w.remove_childs(this);
				return get_components<ComponentType>().remove(h.array_index);
			}
		}
		return false;
	}

	template<typename... ComponentTypes>
	template<typename ComponentType>
	void encomsys<ComponentTypes...>::for_each(void (*func)(const ComponentType&)) {
		for (const component_wrapper<ComponentType>& t : get_components<ComponentType>()) {
			func(t.value);
		}
	}

	template<typename... ComponentTypes>
	template<typename ComponentType>
	void encomsys<ComponentTypes...>::for_each(void (*func)(ComponentType&)) {
		for (component_wrapper<ComponentType>& t : get_components<ComponentType>()) {
			func(t.get_value());
		}
	}

	template<typename... ComponentTypes>
	template<typename ComponentType>
	void encomsys<ComponentTypes...>::for_each(void (*func)(ComponentType&, encomsys& encomsys)) {
		for (const component_wrapper<ComponentType>& t : get_components<ComponentType>()) {
			func(t.value, *this);
		}
	}

	template<typename... ComponentTypes>
	template<typename ComponentType>
	void encomsys<ComponentTypes...>::for_each(void (*func)(ComponentType&, const encomsys& encomsys)) {
		for (const component_wrapper<ComponentType>& t : get_components<ComponentType>()) {
			func(t.value, *this);
		}
	}

	template<typename... ComponentTypes>
	template<typename ComponentType>
	void encomsys<ComponentTypes...>::for_each(void (*func)(const ComponentType&, const encomsys& encomsys)) const {
		// TODO
	}
}


#endif
