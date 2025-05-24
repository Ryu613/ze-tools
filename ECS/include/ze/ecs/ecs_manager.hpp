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
			Entity e{ current_id++ };
			auto archetype = resolveArchetype<TComponents...>();
			archetype->InsertComponentData(e, comps...);
			return e;
		}

	private:
		EcsManager() = default;

		template<typename... TComponents>
		void registerComponentType() {
			(..., (Chunk::component_size[ComponentTypeId::GetTypeId<TComponents>()] = sizeof(TComponents)));
		}

		template<typename... TComponents>
		auto resolveArchetype() {
			ComponentSignature sig;
			// set bitset with each component's type id(an unsigned integer)
			(..., (sig.set(ComponentTypeId::GetTypeId<TComponents>())));
			size_t archetype_key = sig.to_ullong();
			auto it = archetypes.find(archetype_key);
			if (it != archetypes.end()) {
				return it->second.get();
			}
			auto new_archetype = std::make_unique<Archetype>(sig);
			auto ptr = new_archetype.get();
			archetypes.emplace(archetype_key, std::move(new_archetype));
			return ptr;
		}

		uint32_t current_id = 0;
		std::unordered_map<size_t, std::unique_ptr<Archetype>> archetypes;
	};
}
