#ifndef __RELATION_CLASS__
#define __RELATION_CLASS__

#include <tuple>
#include "ref.hpp"

namespace encom {
	struct __relation_tag {};

	template<typename T, typename __Specialization=void>
	struct is_relation {
		static constexpr bool value = std::is_base_of<__relation_tag, T>::value;
	};

	template<typename ...ComponentTypes>
	struct relation : public std::tuple<ComponentTypes...>, __relation_tag {
		using std::tuple<ComponentTypes...>::tuple; // use tuple constructors

		using __component_refs = std::tuple<ref<ComponentTypes>...>;
		using as_ref = std::tuple<ComponentTypes&...>;
	};

	template<typename ...ComponentTypes>
	struct ref_relation : public std::tuple<ref<ComponentTypes>...> {
		using relation_type = relation<ComponentTypes...>;
	};
}

#endif
