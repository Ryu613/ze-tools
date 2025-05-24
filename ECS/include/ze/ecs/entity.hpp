#pragma once

#include <cstdint>

namespace ze::ecs {
	class Entity {
	public:
		using IdType = uint32_t;
		Entity() : id_(0) {}

		bool isNull() const noexcept {
			return id_ == 0;
		}
	private:
		friend class EcsManager;
		Entity(IdType id) : id_(id){}
		IdType id_{ 0 };
	};
} // namespace ze::ecs
