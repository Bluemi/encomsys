#ifndef __REF_CLASS__
#define __REF_CLASS__

#include "util/types.hpp"

namespace encom {
	template<typename T>
	struct ref {
		ID_TYPE consecutive_index;
		ID_TYPE array_index;

		ref(ID_TYPE consecutive_index, ID_TYPE array_index)
			: consecutive_index(consecutive_index), array_index(array_index)
		{ }

		ref()
			: consecutive_index(0), array_index(0)
		{ }
	};
}

#endif
