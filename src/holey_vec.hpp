#ifndef __HOLEY_VEC_CLASS__
#define __HOLEY_VEC_CLASS__

#include <vector>
#include <unordered_set>

namespace __encom_internal {
	using INDEX_TYPE = uint64_t;

	template<typename T>
	class holey_vec;

	template<typename T>
	class holey_vec_iterator {
		private:
			INDEX_TYPE _index;
			INDEX_TYPE _end;
			std::vector<T>* _vec;
			const std::unordered_set<INDEX_TYPE>* const _holes;
		public:
			holey_vec_iterator(
					INDEX_TYPE index,
					INDEX_TYPE end,
					std::vector<T>* vec,
					const std::unordered_set<INDEX_TYPE>* const holes
			)
				: _index(index), _vec(vec), _end(end), _holes(holes)
			{
				// make sure to not start with a hole
				if (points_to_hole()) {
					next();
				}
			}

			bool points_to_hole() const {
				return _holes->find(_index) != _holes->end();
			}

			bool next() {
				while (_index != _end) {
					++_index;
					if (!points_to_hole()) {
						return true;
					}
				}
				return false;
			}

			void operator++() {
				next();
			}

			void operator++(int) {
				next();
			}

			const T& operator*() {
				return (*_vec)[_index];
			}

			bool operator==(const holey_vec_iterator& other) {
				return _index == other._index;
			}

			bool operator!=(const holey_vec_iterator& other) {
				return _index != other._index;
			}
	};

	template<typename T>
	class holey_vec {
		private:
			size_t _size;
		public:
			std::vector<T> _instances;
			std::unordered_set<INDEX_TYPE> _holes;
			holey_vec() : _size(0) {}

			/**
			 * Adds the given t into this vector. If there is an empty slot
			 * this slot is used.
			 *
			 * @param t The instance to add to this vector
			 * @returns The index where the given instance is added
			 */
			INDEX_TYPE add(const T& t) {
				INDEX_TYPE newpos = 0;
				if (_holes.empty()) {
					_instances.push_back(t);
					newpos = _instances.size() - 1;
				} else {
					const std::unordered_set<INDEX_TYPE>::const_iterator b = _holes.begin();
					newpos = *b;
					_instances[newpos] = t;
					_holes.erase(b);
				}
				_size++;
				return newpos;
			}

			bool has_index(const INDEX_TYPE index) const {
				return (index < _instances.size()) && (_holes.find(index) == _holes.end());
			}

			/**
			 * Removes the object at the given position. Adds this index to the
			 * holes.
			 *
			 * @param index The index where to remove the element
			 * @returns false, if there was no element, otherwise true
			 */
			bool remove(INDEX_TYPE index) {
				if (index < _instances.size()) {
					// if index is not in holes
					if (_holes.find(index) == _holes.end()) {
						_holes.insert(index);
						--_size;
						return true;
					}
				}
				return false;
			}

			const T& get(const INDEX_TYPE index) const {
				if (has_index(index)) {
					return _instances[index];
				} else {
					throw 0;
				}
			}

			holey_vec_iterator<T> begin() {
				return holey_vec_iterator<T>(0, _instances.size(), &_instances, &_holes);
			}

			holey_vec_iterator<T> end() {
				return holey_vec_iterator<T>(_instances.size(), _instances.size(), &_instances, &_holes);
			}

			size_t size() const {
				return _size;
			}
	};
}

#endif
