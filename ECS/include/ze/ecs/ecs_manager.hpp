#pragma once

#include <cstdint>
#include <unordered_map>
#include <memory>

#include "ze/ecs/archetype.hpp"
#include "ze/ecs/entity.hpp"

namespace ze::ecs {
	class EcsManager {
	public:
		static EcsManager& Get() {
			static EcsManager mgr;
			return mgr;
		}

		template<typename... TComponents>
		Entity CreateEntity(const TComponents&... comps) {
			// generate id for entity
			Entity e{ next_id_++ };
			Archetype* archetype = resolveArchetype<TComponents...>(e);
			archetype->InsertComponentData(e, comps...);
			return e;
		}

		void DestroyEntity(Entity e) {
			// entity not exist
			if (e.isNull() || e.GetId() >= next_id_) {
				return;
			}
			auto it = entity_to_archetype_.find(e.id_);
			if (it == entity_to_archetype_.end()) {
				return;
			}
			it->second->Remove(e);
			entity_to_archetype_.erase(it);
		}

		template<typename... TComponents, typename... ARGS>
		void AddComponent(Entity e, const ARGS&... comps) {

		}

	private:
		EcsManager() = default;

		template<typename... TComponents>
		Archetype* resolveArchetype(Entity e) {
			ComponentSignature sig;
			// register type and set bitset with each component's type id(an unsigned integer)
			(..., (sig.set(ComponentTypeInfo::GetTypeId<TComponents>())));
			size_t archetype_key = sig.to_ullong();
			auto it = archetypes_.find(archetype_key);
			if (it != archetypes_.end()) {
				entity_to_archetype_.emplace(e.id_, it->second.get());
				return it->second.get();
			}
			auto new_archetype = std::make_unique<Archetype>(sig);
			auto ptr = new_archetype.get();
			//register entity signature
			entity_to_archetype_.emplace(e.id_, ptr);
			archetypes_.emplace(archetype_key, std::move(new_archetype));
			return ptr;
		}

		uint32_t next_id_ = 0;
		std::unordered_map<EntityIdType, Archetype*> entity_to_archetype_;
		std::unordered_map<size_t, std::unique_ptr<Archetype>> archetypes_;
	};
}
