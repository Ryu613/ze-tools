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
	template<typename... Components, typename... ARGS>
	Entity make_entity(ARGS&&... args) {
		return EcsManager::Get().CreateEntity<Components...>(args...);
	}

	// destroy entity
	void destroy_entity(Entity e) {
		return EcsManager::Get().DestroyEntity(e);
	}

	// add component data to exist entity with those components
	// if component doesn't exist in this entity, then throw exception
	template<typename... TComponents, typename... TComponentsData>
	void add_component_data(Entity e, TComponentsData... comps) {

	}

	// add new component to this entity, if exists then throw exception
	template<typename... TComponents, typename... ARGS>
	void add_component(Entity e, TComponents... comps, ARGS&&... args) {

	}

	template<typename... TComponents>
	bool has_components(Entity e) {
		return false;
	}

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
