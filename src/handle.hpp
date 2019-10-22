#ifndef __HANDLE_CLASS__
#define __HANDLE_CLASS__

#include "util/types.hpp"

namespace encom {
	template<typename T>
	struct handle {
		ID_TYPE consecutive_index;
		ID_TYPE array_index;

		handle(ID_TYPE consecutive_index, ID_TYPE array_index)
			: consecutive_index(consecutive_index), array_index(array_index)
		{ }

		handle()
			: consecutive_index(0), array_index(0)
		{ }
	};
}

#endif
