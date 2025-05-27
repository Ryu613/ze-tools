#pragma once

#include <cassert>
#include <cstddef>
#include <cstdlib>

namespace ze {

	// allocate aligned memory
	// ensure memory is aligned in different platform
	inline void* aligned_alloc(size_t size, size_t align) noexcept {
		// for compatibility, align must larger than pointer's size
		align = (align < sizeof(void*)) ? sizeof(void*) : align;
		// align > 0 and must be power of 2
		assert(align && !(align & align - 1));
		// compatibility for POSIX
		assert((align % sizeof(void*)) == 0);
		void* p = nullptr;
		// use system native aligned malloc
#if defined(_WIN32)
		p = ::_aligned_malloc(size, align);
#else
		(void)::posix_memalign(&p, align, size);
#endif
		return p;
	}

	inline void aligned_free(void* p) noexcept {
#if defined(_WIN32)
		::_aligned_free(p);
#else
		::free(p);
#endif
	}


	namespace AllocatorPolicy {
		/**
		* heap allocator: only alloc aligned, not a pool allocator
		*/
		class RawHeapAllocator {
		public:
			RawHeapAllocator() noexcept = default;
			~RawHeapAllocator() noexcept = default;

			template <typename AreaPolicy>
			explicit RawHeapAllocator(const AreaPolicy&&) {}

			void* alloc(size_t size, size_t alignment = alignof(std::max_align_t)) {
				return aligned_alloc(alignment, size);
			}

			void free(void* p) noexcept {
				aligned_free(p);
			}
		};
	} // namespace AllocatorPolicy

	namespace TrackingPolicy {
		class NoTracking {
		public:
			void on_alloc(const void*, size_t, size_t) noexcept {}
		};
	} // namespace TrackingPolicy

	namespace ResourcePolicy {
		class HeapResource {
		public:
			HeapResource() noexcept = default;
			explicit HeapResource(size_t size) {
				if (size) {
					begin_ = malloc(size);
					end_ = static_cast<std::byte*>(begin_) + size;
				}
			}

			~HeapResource() noexcept {
				free(begin_);
			}

			HeapResource(const HeapResource& rhs) = delete;
			HeapResource& operator=(const HeapResource& rhs) = delete;
			HeapResource(HeapResource&& rhs) noexcept = delete;
			HeapResource& operator=(HeapResource&& rhs) noexcept = delete;

			void* data() const noexcept { return begin_; }
			void* begin() const noexcept { return begin_; }
			void* end() const noexcept { return end_; }
			size_t size() const noexcept { return uintptr_t(end_) - uintptr_t(begin_); }

		private:
			void* begin_ = nullptr;
			void* end_ = nullptr;
		};

		class NullResource {
		public:
			void* data() const noexcept { return nullptr; }
			size_t size() const noexcept { return 0; }
		};
	}

	namespace LockingPolicy {
		class NoLock {
		public:
			void lock() noexcept {}
			void unlock() noexcept {}
		};
	}

	/**
	* arena is a contiguous piece of memory, not only heap region
	* it only need allocate once, then manually using it by handing out part of that memory
	* when memory allocation performance is something vital, you can use this allocator
	*
	* AreaPolicy: Heap/Stack/...
	* AllocatorPolicy: HeapAllocator/LinearAllocator/...
	* LockingPolicy: NoLock/Locking
	* TrackingPolicy: NoTrack/...
	*/
	template<
		typename ResourcePolicy,
		typename AllocatorPolicy,
		typename LockingPolicy,
		typename TrackingPolicy
	>
	class Arena {
	public:
		Arena() = default;

		template<typename ... ARGS>
		Arena(const char* name, size_t size, ARGS&& ... args)
			: name_(name),
			mem_resource_(size),
			allocator_(mem_resource_, std::forward<ARGS>(args)...) {
		}

		void* alloc(size_t size, size_t alignment = alignof(std::max_align_t)) noexcept {
			lock_.lock();
			allocator_.alloc(size, alignment);
			lock_.unlock();
		}
	private:
		const char* name_ = nullptr;
		ResourcePolicy mem_resource_;
		AllocatorPolicy allocator_;
		LockingPolicy lock_;
		TrackingPolicy listener_;

	};
}
