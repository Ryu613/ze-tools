#pragma once

#include <bitset>
#include <vector>
#include <array>

#include "ze/ecs/entity.hpp"

namespace ze::ecs {
	constexpr size_t COMPONENT_SIZE = 1 << 6;
	constexpr size_t CHUNK_CAPACITY = 1 << 6;

	using ComponentSignature = std::bitset<COMPONENT_SIZE>;

	// get type id for different Component
	class ComponentTypeId {
	public:
		template<typename T>
		static size_t GetTypeId() {
			static const size_t id = counter++;
			return id;
		}
	private:
		 static inline size_t counter = 0;
	};


	class Chunk {
	public:
		explicit Chunk(const ComponentSignature& sig)
			: signature(sig) {
			for (size_t i = 0; i < COMPONENT_SIZE; ++i) {
				if (sig.test(i)) {
					component_buffers.push_back(::operator new[](CHUNK_CAPACITY* component_size[i]));
				}
			}
		}

		~Chunk() {
			for (void* buf : component_buffers) {
				::operator delete[](buf);
			}
		}

		size_t GetCount() {
			return count;
		}

		template<typename TComponent>
		void InsertComponentData(Entity e, const TComponent& comp) {
			size_t type_id = ComponentTypeId::GetTypeId<TComponent>();
			size_t buf_index = 0;
			for (size_t i = 0; i < type_id; ++i) {
				if (signature.test(i)) {
					++buf_index;
				}
			}
			size_t index = count++;
			entities[index] = e;
			void* raw_buf = component_buffers[buf_index];
			new (static_cast<char*>(raw_buf) + index * sizeof(TComponent)) TComponent(comp);
		}

		// component size mapped by component type id
		static inline std::array<size_t, COMPONENT_SIZE> component_size;

	private:
		size_t count = 0;

		std::array<Entity, CHUNK_CAPACITY> entities;

		ComponentSignature signature;
		// to mark each data belongs to which entity
		// buffers for each component type of archetype
		std::vector<void*> component_buffers;
	};

	class Archetype {
	public:
		explicit Archetype(ComponentSignature sig)
			: signature(sig) {
		}

		Chunk* GetOrCreateChunk() {
			if (!chunks.empty() && chunks.back()->GetCount() < CHUNK_CAPACITY) {
				return chunks.back().get();
			}
			chunks.emplace_back(std::make_unique<Chunk>(signature));
			return chunks.back().get();
		}

		template<typename... TComponents>
		void InsertComponentData(Entity e, const TComponents&... comps) {
			Chunk* chunk = GetOrCreateChunk();
			// insert components data to chunk
			(chunk->InsertComponentData<TComponents>(e, comps), ...);
		}

	private:
		ComponentSignature signature;
		std::vector<std::unique_ptr<Chunk>> chunks;
	};
}
