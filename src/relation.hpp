#ifndef __RELATION_CLASS__
#define __RELATION_CLASS__

#include <tuple>
#include "handle.hpp"

namespace encom {
	struct __relation_tag {};

	template<typename T, typename __Specialization=void>
	struct is_relation {
		static constexpr bool value = std::is_base_of<__relation_tag, T>::value;
	};

	template<typename T>
	inline constexpr bool is_relation_v = is_relation<T>::value;

	/**
	 * relation_ref_expander transforms every relation R to R::as_ref and any other type T to T&
	 */
	template<typename T, typename S=void>
	struct relation_ref_expander {
		using type = T&;
	};

	template<typename T>
	struct relation_ref_expander<T, std::enable_if_t<is_relation_v<T>>> {
		using type = typename T::as_ref;
	};

	template<typename T>
	using relation_ref_expander_t = typename relation_ref_expander<T>::type;

	template<typename ...Ts>
	using __last = std::tuple_element_t<sizeof...(Ts)-1, std::tuple<Ts...>>;

	template<typename ...Ts>
	using __first = std::tuple_element_t<0, std::tuple<Ts...>>;

	template<typename ...ComponentTypes>
	struct relation : public std::tuple<ComponentTypes...>, __relation_tag {
		// use tuple constructors
		using std::tuple<ComponentTypes...>::tuple;

		// type definitions
		using __component_handles = std::tuple<handle<ComponentTypes>...>;

		struct as_ref : public std::tuple<relation_ref_expander_t<ComponentTypes>...> {
			as_ref(const std::tuple<relation_ref_expander_t<ComponentTypes>...>& t) : std::tuple<relation_ref_expander_t<ComponentTypes>...>(t) {}

			// as_ref get function
			template<size_t I = 0, typename ...Ts>
			typename std::enable_if_t<
				I == sizeof...(Ts) - 1,
				const relation_ref_expander_t<__last<Ts...>>
			> get_helper(const relation_ref_expander_t<__last<Ts...>> element) const {
				return element;
			}

			template<size_t I = 0, typename ...Ts>
			typename std::enable_if_t<
				I < sizeof...(Ts) - 1,
				const relation_ref_expander_t<__last<Ts...>>
			> get_helper(const relation_ref_expander_t<std::tuple_element_t<I, std::tuple<Ts...>>>& element) const {
				using CurrentComponentType = relation_ref_expander_t<std::tuple_element_t<I+1, std::tuple<Ts...>>>;
				return get_helper<I+1, Ts...>(std::get<CurrentComponentType>(element));
			}

			template<typename ...Ts>
			relation_ref_expander_t<__last<Ts...>> get() {
				return const_cast<relation_ref_expander_t<__last<Ts...>>>(const_cast<const relation<ComponentTypes...>::as_ref&>(*this).get<Ts...>());
			}

			template<typename ...Ts>
			const relation_ref_expander_t<__last<Ts...>> get() const {
				using CurrentComponentType = relation_ref_expander_t<__first<Ts...>>;
				return get_helper<0, Ts...>(std::get<CurrentComponentType>(*this));
			}
		};

		// get function
		template<size_t I = 0, typename ...Ts>
		typename std::enable_if_t<
			I == sizeof...(Ts) - 1,
			const __last<Ts...>
		>& get_helper(const __last<Ts...>& element) const {
			return element;
		}

		template<size_t I = 0, typename ...Ts>
		typename std::enable_if_t<
			I < sizeof...(Ts) - 1,
			const __last<Ts...>
		>& get_helper(const std::tuple_element_t<I, std::tuple<Ts...>>& element) const {
			using current_component_type = std::tuple_element_t<I+1, std::tuple<Ts...>>;
			return get_helper<I+1, Ts...>(std::get<current_component_type>(element));
		}

		template<typename ...Ts>
		__last<Ts...>& get() {
			return const_cast<const relation<ComponentTypes...>&>(*this).get<Ts...>();
		}

		template<typename ...Ts>
		const __last<Ts...>& get() const {
			using CurrentComponentType = std::tuple_element_t<0, std::tuple<Ts...>>;
			return get_helper<0, Ts...>(std::get<CurrentComponentType>(*this));
		}
	};
}

#endif
