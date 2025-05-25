#pragma once

#include <cstddef>
#include <bitset>
#include <vector>
#include <array>
#include <type_traits>
#include <cassert>

#include "ze/ecs/entity.hpp"

namespace ze::ecs {
	constexpr size_t COMPONENT_SIZE = 1 << 6;
	constexpr size_t CHUNK_CAPACITY = 1 << 6;

	using ComponentSignature = std::bitset<COMPONENT_SIZE>;

	using EntityIdType = Entity::IdType;

	// componentTypeInfo
	// only support trivial component for now
	class ComponentTypeInfo {
	public:
		template<typename T>
		static size_t GetTypeId() {
			static const size_t id = counter++;
			registerType<T>(id);
			return id;
		}

		static size_t GetSize(size_t type_id) {
			return component_sizes[type_id];
		}

		//static void Destroy(size_t type_id, void* ptr) {
		//	component_vtables[type_id].destroy(ptr);
		//}

	private:

		//struct VTable {
		//	void (*destroy)(void*) = nullptr;
		//};

		template<typename T>
		static bool registerType(size_t id) noexcept {
			static_assert(std::is_trivially_destructible_v<T>,
				"Component must be trivially destructible (no std::string, std::vector, etc.)");
			component_sizes[id] = sizeof(T);
			//component_vtables[id].destroy = [](void* p) { reinterpret_cast<T*>(p)->~T(); };
			return true;
		}

		static inline std::array<size_t, COMPONENT_SIZE> component_sizes{};
		//static inline std::array<VTable, COMPONENT_SIZE> component_vtables{};
		static inline size_t counter = 0;
	};


	class Chunk {
	public:
		explicit Chunk(const ComponentSignature& sig)
			: signature_(sig) {
			entity_ids_.fill(std::numeric_limits<EntityIdType>::max());
			for (size_t type_id = 0; type_id < COMPONENT_SIZE; ++type_id) {
				if (sig.test(type_id)) {
					component_buffers_[type_id] = (::operator new[](CHUNK_CAPACITY * ComponentTypeInfo::GetSize(type_id)));
				}
			}
		}

		~Chunk() {
			for (size_t type_id = 0; type_id < COMPONENT_SIZE; ++type_id) {
				if (signature_.test(type_id)) {
					std::byte* raw_buf_ptr = static_cast<std::byte*>(component_buffers_[type_id]);
					size_t type_size = ComponentTypeInfo::GetSize(type_id);
					//for (size_t j = 0; j < count; ++j) {
					//	ComponentTypeInfo::Destroy(type_id, raw_buf_ptr + j * type_size);
					//}
					::operator delete[](component_buffers_[type_id]);
				}
			}
		}

		size_t GetCount() {
			return count;
		}

		template<typename TComponent>
		void InsertComponentData(Entity e, const TComponent& comp) {
			size_t type_id = ComponentTypeInfo::GetTypeId<TComponent>();
			assert(signature_.test(type_id) && "Component type not in this Chunk");
			size_t index = count++;
			entity_ids_[index] = e.GetId();
			void* raw_buf = component_buffers_[type_id];
			new (static_cast<std::byte*>(raw_buf) + index * ComponentTypeInfo::GetSize(type_id)) TComponent(comp);
		}

		void Remove(Entity e) {
			for (size_t i = 0; i < count; ++i) {
				if (entity_ids_[i] == e.GetId()) {
					--count;
					// if entity is not the last one of entities, swap entity id for keeping memory density
					if (i != count) {
						entity_ids_[i] = entity_ids_[count];					
					}
					for (size_t type_id = 0; type_id < COMPONENT_SIZE; ++type_id) {
						if (!signature_.test(type_id)) {
							continue;
						}
						size_t type_size = ComponentTypeInfo::GetSize(type_id);
						std::byte* dst = static_cast<std::byte*>(component_buffers_[type_id]) + i * type_size;
						std::byte* src = static_cast<std::byte*>(component_buffers_[type_id]) + count * type_size;
						// invoke destructor of component
						//ComponentTypeInfo::Destroy(type_id, dst);
						// swap the last of entity's data to removed entity's place
						std::memcpy(dst, src, type_size);
					}
					return;
				}
			}
		}


	private:
		ComponentSignature signature_;
		// how many data this chunk has
		size_t count = 0;
		// to mark each data belongs to which entity
		std::array<EntityIdType, CHUNK_CAPACITY> entity_ids_;
		// buffers for each component type of archetype
		std::array<void*, COMPONENT_SIZE> component_buffers_{};
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
