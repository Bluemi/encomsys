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
	using is_relation_v = typename is_relation<T>::value;

	/**
	 * __relation_ref_expander transforms every relation R to R::as_ref and any other type T to T
	 */
	template<typename T, typename S=void>
	struct __relation_ref_expander {
		using type = T;
	};

	template<typename T>
	struct __relation_ref_expander<T, std::enable_if_t<is_relation<T>::value>> {
		using type = typename T::as_ref;
	};

	template<typename ...ComponentTypes>
	struct relation : public std::tuple<ComponentTypes...>, __relation_tag {
		using std::tuple<ComponentTypes...>::tuple; // use tuple constructors

		using __component_handles = std::tuple<handle<ComponentTypes>...>;
		using as_ref = std::tuple<__relation_ref_expander<ComponentTypes>&...>;
	};

	template<typename ...ComponentTypes>
	struct ref_relation : public std::tuple<handle<ComponentTypes>...> {
		using relation_type = relation<ComponentTypes...>;
	};
}

#endif
