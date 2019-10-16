#ifndef __INDEX_VECTOR_CLASS__
#define __INDEX_VECTOR_CLASS__

#include <vector>
#include <unordered_set>

namespace __encom_internal {
	using INDEX_TYPE = uint64_t;

	template<typename T>
	class index_vector_iterator {
		private:
			INDEX_TYPE _index;
			INDEX_TYPE _end;
			const std::vector<T>* const _vec;
			const std::unordered_set<INDEX_TYPE>* const _holes;
		public:
			index_vector_iterator(
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

			T& operator*() {
				return (*_vec)[_index];
			}

			bool operator==(const index_vector_iterator& other) const {
				return _index == other._index;
			}

			bool operator!=(const index_vector_iterator& other) const {
				return _index != other._index;
			}
	};

	template<typename T>
	class const_index_vector_iterator {
		private:
			INDEX_TYPE _index;
			INDEX_TYPE _end;
			const std::vector<T>* const _vec;
			const std::unordered_set<INDEX_TYPE>* const _holes;

		public:
			const_index_vector_iterator(
					INDEX_TYPE index,
					INDEX_TYPE end,
					const std::vector<T>* const vec,
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

			const T& operator++() {
				next();
				return (*_vec)[_index];
			}

			const T& operator++(int) {
				T* last = &(*_vec)[_index];
				next();
				return *last;
			}

			const T& operator*() const {
				return (*_vec)[_index];
			}

			bool operator==(const const_index_vector_iterator& other) const {
				return _index == other._index;
			}

			bool operator!=(const const_index_vector_iterator& other) const {
				return _index != other._index;
			}
	};

	/**
	 * This implements a container that saves elements at an index. This index
	 * is not changing, even if elements before this element are removed. If
	 * an element is removed it leaves an empty slot. New elements are inserted
	 * at these empty slots.
	 */
	template<typename T>
	class index_vector {
		private:
			size_t _size;
			std::vector<T> _instances;
			std::unordered_set<INDEX_TYPE> _holes;
		public:
			using iterator = index_vector_iterator<T>;
			using const_iterator = const_index_vector_iterator<T>;

			/**
			 * Constructs a new index vector with no elements.
			 */
			index_vector() : _size(0) {}

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

			/**
			 * Returns whether this index holds an element.
			 * @param index The index to check
			 * @returns true, if there is an element at the specified index, otherwise false
			 */
			bool has_index(const INDEX_TYPE index) const {
				return (index < _instances.size()) && (_holes.find(index) == _holes.end());
			}

			/**
			 * Removes the object at the given position. Adds this index to the
			 * holes. If there is no element at the specified index. Nothing happens and
			 * false is returned.
			 *
			 * @param index The index where to remove the element
			 * @returns true, if there was an element at the specified index, otherwise false
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

			/**
			 * Returns the element at the specified position. If there is no element at the
			 * specified index an exception is thrown.
			 *
			 * @param index The index specifying the element to get
			 * @returns The element at the given index
			 */
			const T& get(const INDEX_TYPE index) const {
				if (has_index(index)) {
					return _instances[index];
				} else {
					throw "Tried to get invalid index";
				}
			}

			/**
			 * Returns the element at the specified position. If there is no element at the
			 * specified index an exception is thrown.
			 *
			 * @param index The index specifying the element to get
			 * @returns The element at the given index
			 */
			T& get(const INDEX_TYPE index) {
				if (has_index(index)) {
					return _instances[index];
				} else {
					throw "Tried to get invalid index";
				}
			}

			/**
			 * @returns an read/write iterator pointing to the start of this index_vector.
			 */
			iterator begin() {
				return index_vector_iterator<T>(0, _instances.size(), &_instances, &_holes);
			}

			/**
			 * @returns an read/write iterator pointing to the end of this index_vector.
			 */
			iterator end() {
				return index_vector_iterator<T>(_instances.size(), _instances.size(), &_instances, &_holes);
			}

			/**
			 * @returns an read-only iterator pointing to the start of this index_vector.
			 */
			const_iterator begin() const {
				return const_index_vector_iterator<T>(0, _instances.size(), &_instances, &_holes);
			}

			/**
			 * @returns an read-only iterator pointing to the end of this index_vector.
			 */
			const_iterator end() const {
				return const_index_vector_iterator<T>(_instances.size(), _instances.size(), &_instances, &_holes);
			}

			/**
			 * @returns the number of elements in this vector. Be aware that there can be a valid index, that is bigger
			 * 			than this size.
			 */
			size_t size() const {
				return _size;
			}
	};
}

#endif
