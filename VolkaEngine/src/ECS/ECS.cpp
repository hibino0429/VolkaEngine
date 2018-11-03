#include "ECS.hpp"

void ECS::Entity::addGroup(Group group) noexcept
{
	groupBitSet[group] = true;
	manager_.addToGroup(this, group);
}