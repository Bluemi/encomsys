#ifndef __ENCOMSYS_CLASS__
#define __ENCOMSYS_CLASS__

#include <tuple>
#include <iostream>

#include "util/index_vector.hpp"
#include "util/types.hpp"
#include "handle.hpp"
#include "relation.hpp"

namespace encom {
	template<typename ...ComponentTypes>
	class encomsys;

	template<typename ComponentType, typename __Specialization=void>
	class component_wrapper {
		public:
			ID_TYPE consecutive_index;
			ComponentType value;

			component_wrapper(ID_TYPE consecutive_index, const ComponentType& value)
				: consecutive_index(consecutive_index), value(value)
			{ }

			ComponentType& get_value() {
				return value;
			}

			const ComponentType& get_value() const {
				std::cout << "component_wrapper<" << typeid(ComponentType).name() << ">::get_value()" << std::endl;
				return value;
			}
	};

	/**
	 * This specialization contains the relations sub type __component_handles, to point to
	 * the components locations.
	 */
	template<typename RelationType>
	struct component_wrapper<RelationType, std::enable_if_t<is_relation<RelationType>::value>> {
		ID_TYPE consecutive_index;
		typename RelationType::__component_handles _handles;

		component_wrapper(ID_TYPE consecutive_index, const typename RelationType::__component_handles& handles) : consecutive_index(consecutive_index), _handles(handles) {}

		template<size_t I = 0, typename ...RelationComponentTypes, typename ...ComponentTypes>
		std::enable_if_t<I == sizeof...(RelationComponentTypes)> relation_get_helper(
			std::tuple<RelationComponentTypes...>*,
			const encomsys<ComponentTypes...>* const
		) const { }

		template<size_t I = 0, typename ...RelationComponentTypes, typename ...ComponentTypes>
		std::enable_if_t<I < sizeof...(RelationComponentTypes)>
		relation_get_helper(
			std::tuple<RelationComponentTypes...>* relation,
			const encomsys<ComponentTypes...>* const encomsys
		) const {
			std::cout << "relation_get_helper<I=" << I << ", sizeof...(RelationComponentTypes)=" << sizeof...(RelationComponentTypes) << ">()" << std::endl;
			using current_relation_component_type = typename std::tuple_element<I, std::tuple<RelationComponentTypes...>>::type;
			std::get<I>(*relation) = *encomsys->get(std::get<I>(_handles));
			relation_get_helper<I+1>(relation, encomsys);
		}

		/**
		 * Returns the relation instance as object, without references.
		 */
		template<typename ...ComponentTypes>
		RelationType get_value(const encomsys<ComponentTypes...>* const encomsys) const {
			std::cout << "component_wrapper<" << typeid(RelationType).name() << ">::get_value()" << std::endl;
			RelationType relation;
			relation_get_helper(&relation, encomsys);
			return relation;
		}
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
			 * @returns a handle to the added component
			 */
			template<typename ComponentType>
			std::enable_if_t<!is_relation<ComponentType>::value, handle<ComponentType>> add(const ComponentType& component);

			/**
			 * Adds the given relation into this encomsys.
			 *
			 * @param relation_component The relation component to add to this encomsys
			 * @returns a handle to the added relation
			 */
			template<typename RelationType>
			std::enable_if_t<is_relation<RelationType>::value, handle<RelationType>> add(const RelationType& relation_component);

			/**
			 * @param handle The reference to the requests component
			 * @returns the component referenced by the given handle. If the component could not be found
			 * 			nullptr is returned.
			 */
			template<typename ComponentType>
			std::enable_if_t<!is_relation<ComponentType>::value, std::optional<ComponentType>>
			get(const handle<ComponentType>&) const;

			/**
			 * @param handle The reference to the requests component
			 * @returns the component referenced by the given handle. If the component could not be found
			 * 			nullptr is returned.
			 */
			template<typename RelationType>
			std::enable_if_t<is_relation<RelationType>::value, std::optional<RelationType>>
			get(const handle<RelationType>&) const;

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
	std::enable_if_t<!is_relation<ComponentType>::value, handle<ComponentType>> encomsys<ComponentTypes...>::add(const ComponentType& component) {
		const component_wrapper<ComponentType> w(_next_consecutive_id, component);
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
		std::tuple<handle<RelationComponentTypes>...>* references,
		encomsys<ComponentTypes...>* encomsys
	) {
		using component_type = typename std::tuple_element<I, std::tuple<RelationComponentTypes...>>::type;
		handle<component_type> component_ref = encomsys->add(std::get<I>(relation_components));
		add_relation_components<I+1>(relation_components, references, encomsys);
		std::get<handle<component_type>>(*references) = component_ref;
	}

	template<typename ...RelationComponentTypes, typename ...ComponentTypes>
	std::tuple<handle<RelationComponentTypes>...> relation_add_helper(const std::tuple<RelationComponentTypes...>& relation_components, encomsys<ComponentTypes...>* encomsys) {
		std::tuple<handle<RelationComponentTypes>...> references;
		add_relation_components(relation_components, &references, encomsys);
		return references;
	}

	template<typename... ComponentTypes>
	template<typename RelationType>
	std::enable_if_t<is_relation<RelationType>::value, handle<RelationType>> encomsys<ComponentTypes...>::add(const RelationType& relation_component) {
		typename RelationType::__component_handles references = relation_add_helper(relation_component, this);

		component_wrapper<RelationType> w(_next_consecutive_id, references);

		ID_TYPE array_index = get_components<RelationType>().add(w);

		return handle<RelationType>(_next_consecutive_id++, array_index);
	}

	template<typename... ComponentTypes>
	template<typename ComponentType>
	std::enable_if_t<!is_relation<ComponentType>::value, std::optional<ComponentType>>
	encomsys<ComponentTypes...>::get(const handle<ComponentType>& component_handle) const {
		if (has_element(component_handle)) {
			return get_components<ComponentType>().get(component_handle.array_index).get_value();
		}
		return {};
	}

	template<typename... ComponentTypes>
	template<typename RelationType>
	std::enable_if_t<
		is_relation<RelationType>::value,
		std::optional<RelationType>
	>
	encomsys<ComponentTypes...>::get(const handle<RelationType>& relation_handle) const {
		if (has_element(relation_handle)) {
			return std::optional(get_components<RelationType>().get(relation_handle.array_index).get_value(this));
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
	bool encomsys<ComponentTypes...>::remove(const handle<ComponentType>& r) {
		if (has_element(r)) {
			return get_components<ComponentType>().remove(r.array_index);
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
