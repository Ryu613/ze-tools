#pragma once

#include <vector>
#include <limits>
#include <cstdint>
#include <algorithm>

namespace ze::collection {
	template<typename T = uint32_t>
    class sparse_set {
    public:
        static constexpr T NIL = std::numeric_limits<T>::max();

        sparse_set(size_t cap = 64) {
            dense_.reserve(cap);
            sparse_.assign(cap, NIL);
        }

        bool contains(T x) const {
            return x < sparse_.size() && sparse_[x] != NIL;
        }

        void insert(T x) {
            if (contains(x)) return;
            if (x >= sparse_.size()) {
                size_t new_size = std::max(static_cast<size_t>(x) + 1, sparse_.size() * 2);
                sparse_.resize(new_size, NIL);
            }
            sparse_[x] = dense_.size();
            dense_.push_back(x);
        }

        void erase(T x) {
            if (!contains(x)) return;
            size_t idx = sparse_[x];
            T last = dense_.back();
            dense_[idx] = last;
            sparse_[last] = idx;
            dense_.pop_back();
            sparse_[x] = NIL;
        }

        void clear() {
            for (T x : dense_) sparse_[x] = NIL;
            dense_.clear();
        }

        size_t Size() const { return dense_.size(); }
        bool empty() const { return dense_.empty(); }


        using iterator = typename std::vector<T>::iterator;
        using const_iterator = typename std::vector<T>::const_iterator;

        iterator begin() { return dense_.begin(); }
        iterator end() { return dense_.end(); }
        const_iterator begin() const { return dense_.begin(); }
        const_iterator end() const { return dense_.end(); }
        const_iterator cbegin() const { return dense_.cbegin(); }
        const_iterator cend() const { return dense_.cend(); }
        iterator find(T x) {
            if (!contains(x)) return end();
            return begin() + sparse_[x];
        }

    private:
        std::vector<T> dense_;
        std::vector<T> sparse_;
    };
} // namespace ze::collection