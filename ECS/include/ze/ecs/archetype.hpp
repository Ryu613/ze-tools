#pragma once

#include <bitset>
#include <vector>
#include <array>
#include <limits>
#include <functional>

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
			: signature_(sig) {
			for (size_t type_id = 0; type_id < COMPONENT_SIZE; ++type_id) {
				if (sig.test(type_id)) {
					buffer_type_ids.push_back(type_id);
					component_buffers.push_back(::operator new[](CHUNK_CAPACITY * component_sizes[type_id]));
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
			auto it = std::find(buffer_type_ids.begin(), buffer_type_ids.end(), type_id);
			if (it == buffer_type_ids.end()) {
				throw std::runtime_error("Component type not in this Chunk");
			}
			size_t buf_index = it - buffer_type_ids.begin();
			size_t index = count++;
			entities[index] = e;
			void* raw_buf = component_buffers[buf_index];
			new (static_cast<char*>(raw_buf) + index * component_sizes[type_id]) TComponent(comp);
		}

		void Remove(Entity e) {
			for (size_t i = 0; i < count; ++i) {
				if (entities[i] == e) {
					--count;
					// if entity is not the last one of entities, swap entity id for keeping memory density
					if (i != count) {
						entities[i] = entities[count];
						for (size_t j = 0; j < component_buffers.size(); ++j) {
							size_t type_id = buffer_type_ids[j];
							// swap the last of entity's data to removed entity's place
							std::memcpy(
								static_cast<char*>(component_buffers[j]) + i * component_sizes[type_id],
								static_cast<char*>(component_buffers[j]) + count * component_sizes[type_id],
								component_sizes[type_id]
							);
							std::function<void(void*)> destructors[COMPONENT_SIZE];

						}
					}
					return;
				}
			}
		}

		// component size mapped by component type id
		static inline std::array<size_t, COMPONENT_SIZE> component_sizes;

	private:
		ComponentSignature signature_;
		// how much data this chunk has
		size_t count = 0;
		// to mark each data belongs to which entity
		std::array<Entity, CHUNK_CAPACITY> entities;
		// buffer and type_id reference
		std::vector<size_t> buffer_type_ids;
		// buffers for each component type of archetype
		std::vector<void*> component_buffers;
	};

	class Archetype {
	public:
		explicit Archetype(ComponentSignature sig)
			: signature_(sig) {
		}

		Chunk* GetOrCreateChunk() {
			if (!chunks_.empty() && chunks_.back()->GetCount() < CHUNK_CAPACITY) {
				return chunks_.back().get();
			}
			chunks_.emplace_back(std::make_unique<Chunk>(signature_));
			return chunks_.back().get();
		}

		template<typename... TComponents>
		void InsertComponentData(Entity e, const TComponents&... comps) {
			Chunk* chunk = GetOrCreateChunk();
			// insert components data to chunk
			(...,(chunk->InsertComponentData<TComponents>(e, comps)));
		}


		void Remove(Entity e) {
			for (auto& chunk : chunks_) {
				chunk->Remove(e);
			}
		}

	private:
		ComponentSignature signature_;
		std::vector<std::unique_ptr<Chunk>> chunks_;
	};
}
