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
	void add_component(Entity e, ARGS&&... args) {

	}

	// return true if all components are exist
	template<typename... TComponents>
	bool has_components(Entity e) {
		return false;
	}

	// remove component from this entity, if component not exists then do nothing
	template<typename... TComponents>
	void remove_components(Entity e) {

	}

	template<typename... TSystems>
	void register_system() {

	}

	template<typename... TSystem>
	void system_update() {

	}

	void system_update_all() {

	}

	template<typename TSystem>
	void enable_system() {

	}

	template<typename TSystem>
	void disable_system() {

	}
	
	template<typename... TComponents>
	void query() {
	}
} // namespace ze::ecs
