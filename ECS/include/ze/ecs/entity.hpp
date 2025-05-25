#pragma once

#include <cstdint>
#include <limits>

namespace ze::ecs {
	constexpr uint32_t NULL_ID = std::numeric_limits<uint32_t>::max();
	class Entity {
	public:
		using IdType = uint32_t;
		Entity() {}

		bool isNull() const noexcept {
			return id_ == NULL_ID;
		}

		bool operator==(const Entity& e) {
			return id_ == e.id_;
		}

	private:
		friend class EcsManager;
		Entity(IdType id) : id_(id){}
		IdType id_{ NULL_ID };
	};
} // namespace ze::ecs
