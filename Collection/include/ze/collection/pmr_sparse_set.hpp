#include <vector>
#include <limits>
#include <cstdint>
#include <algorithm>
#include <memory_resource>

namespace ze::collection::pmr {
    template<typename T = uint32_t>
    class sparse_set {
    public:
        using allocator_type = std::pmr::polymorphic_allocator<T>;
        static constexpr T NIL = std::numeric_limits<T>::max();

        sparse_set(size_t cap = 64, allocator_type alloc = {})
            : dense_{ alloc }, sparse_{ alloc }
        {
            dense_.reserve(cap);
            sparse_.resize(cap, NIL);
        }

        void insert(T x) {
            if (contains(x)) return;
            if (x >= sparse_.size()) {
                sparse_.resize(std::max(static_cast<size_t>(x) + 1, sparse_.size() * 2), NIL);
            }
            sparse_[x] = dense_.size();
            dense_.push_back(x);
        }

        bool contains(T x) const {
            return x < sparse_.size() && sparse_[x] < dense_.size() && dense_[sparse_[x]] == x;
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

        size_t size() const { return dense_.size(); }

        using iterator = typename std::pmr::vector<T>::iterator;
        using const_iterator = typename std::pmr::vector<T>::const_iterator;

        iterator begin() { return dense_.begin(); }
        iterator end() { return dense_.end(); }
        const_iterator begin() const { return dense_.begin(); }
        const_iterator end() const { return dense_.end(); }

    private:
        std::pmr::vector<T> dense_;
        std::pmr::vector<T> sparse_;
    };
}
