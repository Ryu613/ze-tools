#pragma once

#include <memory>
#include <vector>
#include <limits>
#include <cstdint>

namespace ze::collection {
	class SparseSet {
		using ElemType = uint32_t;
		using ArrayType = std::vector<ElemType>;
		static inline size_t DEFAULT_CAPACITY = 1 << 6;
		static inline ElemType NULL_VALUE = std::numeric_limits<ElemType>::max();
	public:
		SparseSet(size_t capacity = DEFAULT_CAPACITY)
			: dense_(std::make_unique<ArrayType>(capacity, NULL_VALUE)),
			  sparse_(std::make_unique<ArrayType>(capacity, NULL_VALUE)) {
		}

		void Insert(ElemType elem) {
			size_t index = count_++;
			if (!checkCapacity(elem)) {
				resize(capacity_ << 1);
			}
			(*dense_)[index] = elem;
			(*sparse_)[elem] = index;
		}

		void Remove(ElemType e) {
			if (!HasElement(e)) {
				return;
			}
			auto& dense = *dense_;
			auto& sparse = *sparse_;
			if (count_ == 1) {
				dense[0] = NULL_VALUE;
				sparse[e] = NULL_VALUE;
				count_--;
				return;
			}
			std::swap(dense[sparse[e]], dense[--count_]);
			sparse[e] = NULL_VALUE;
		}

		bool HasElement(ElemType e) {
			return (*sparse_)[e] != NULL_VALUE;
		}

		size_t Size() {
			return count_;
		}

	private:
		size_t capacity_ = DEFAULT_CAPACITY;
		size_t count_ = 0;
		std::unique_ptr<ArrayType> dense_;
		std::unique_ptr<ArrayType> sparse_;

		bool checkCapacity(size_t c) {
			return c <= capacity_;
		}

		void resize(size_t s) {
			dense_->resize(s, NULL_VALUE);
			sparse_->resize(s, NULL_VALUE);
		}
	};
} // namespace ze::collection