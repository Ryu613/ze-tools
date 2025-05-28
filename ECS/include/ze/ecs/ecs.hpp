#pragma once

#include "ze/ecs/entity.hpp"
#include "ze/ecs/system.hpp"
#include "ze/ecs/ecs_manager.hpp"

namespace ze::ecs {
	// init whole ecs system
	void init() {
		EcsManager::Get();
	}

	// create new entity
	template<typename... TComponents, typename... ARGS>
	Entity make_entity(ARGS&&... args) {
		return EcsManager::Get().CreateEntity<TComponents...>(args...);
	}

	// destroy entity
	void destroy_entity(Entity e) {
		EcsManager::Get().DestroyEntity(e);
	}

	// add component data to exist entity with those components
	// if component doesn't exist in this entity, then add new component
	template<typename... TComponents, typename... ARGS>
	void add_components(const Entity& e, ARGS&&... args) {
		EcsManager::Get().AddComponent<TComponents...>(e, args...);
	}

	// return true if all components are exist
	template<typename... TComponents>
	bool has_components(const Entity& e) {
		return EcsManager::Get().HasComponent<TComponents...>(e);
	}

	// remove component from this entity, if component not exists then do nothing
	template<typename... TComponents>
	void remove_components(const Entity& e) {
		EcsManager::Get().RemoveComponent<TComponents...>(e);
	}

	template<typename... TSystems>
	void register_systems() {
		EcsManager::Get().RegisterSystem<TSystems...>();
	}

	template<typename... TSystems>
	void system_update() {
		EcsManager::Get().TriggerSystemUpdate<TSystems...>();
	}

	void system_update_all() {
		EcsManager::Get().TriggerSystemUpdate();
	}

	template<typename... TSystems>
	void enable_systems() {
		EcsManager::Get().EnableSystem<TSystems...>();
	}

	template<typename... TSystems>
	void disable_systems() {
		EcsManager::Get().DisableSystem<TSystems...>();
	}
	
	template<typename... TComponents>
	void query() {
	}
} // namespace ze::ecs
