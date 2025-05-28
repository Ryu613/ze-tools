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
			// TODO prefer to use recycled id
			Entity e{ next_id_++ };
			Archetype* archetype = resolveArchetype<TComponents...>(e);
			archetype->InsertComponentData(e, comps...);
			return e;
		}

		void DestroyEntity(Entity e) {
			if (!isValidEntity(e)) {
				return;
			}
			auto it = entity_to_archetype_.find(e.id_);
			if (it == entity_to_archetype_.end()) {
				return;
			}
			it->second->Remove(e);
			entity_to_archetype_.erase(it);
			// TODO recyle id
		}

		template<typename... TComponents, typename... ARGS>
		void AddComponent(const Entity& e, const ARGS&... comps) {
			if (!isValidEntity(e)) {
				return;
			}
			auto it = entity_to_archetype_.find(e.id_);
			if (it == entity_to_archetype_.end()) {
				return;
			}
			it->second->InsertComponentData<TComponents...>(e, comps...);
		}

		template<typename... TComponents>
		bool HasComponent(const Entity& e) {
			if (!isValidEntity(e)) {
				return false;
			}
			auto it = entity_to_archetype_.find(e.id_);
			if (it == entity_to_archetype_.end()) {
				return false;
			}
			const auto& sig = it->second->GetSignature();
			bool finded = false;
			(..., (finded = sig.test(ComponentTypeInfo::GetTypeId<TComponents>())));
			return finded;
		}

		template<typename... TComponents>
		void RemoveComponent(const Entity& e) {
			if (!isValidEntity(e)) {
				return;
			}
			auto it = entity_to_archetype_.find(e.id_);
			if (it == entity_to_archetype_.end()) {
				return;
			}
			auto old_sig = it->second->GetSignature().to_ullong();
			(..., (it->second->RemoveComponent<TComponents>(e)));
			auto updated_sig = it->second->GetSignature().to_ullong();
			auto a_it = archetypes_.find(old_sig);
			assert(a_it == archetypes_.end() && "cannot find archetype's signature!");
			auto node = archetypes_.extract(old_sig);
			node.key() = updated_sig;
			archetypes_.insert(std::move(node));
		}

		template<typename... TSystems>
		void RegisterSystem() {

		}

		template<typename... TSystems>
		void TriggerSystemUpdate() {

		}

		template<typename... TSystems>
		void EnableSystem() {

		}

		template<typename... TSystems>
		void DisableSystem() {

		}

	private:
		EcsManager() = default;

		template<typename... TComponents>
		Archetype* resolveArchetype(const Entity& e) {
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
		
		bool isValidEntity(const Entity& e) {
			if (e.isNull() || e.GetId() >= next_id_) {
				return false;
			}
			return true;
		}

		uint32_t next_id_ = 0;

		std::unordered_map<EntityIdType, Archetype*> entity_to_archetype_;
		std::unordered_map<size_t, std::unique_ptr<Archetype>> archetypes_;
	};
}
