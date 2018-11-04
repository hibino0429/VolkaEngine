//---------------------------------------------------------
//!@file Collider.hpp
//!@brief Collider‚ðˆµ‚¢‚Ü‚·
//!@author Volka
//!@date 2018/11/3
//---------------------------------------------------------
#pragma once
#include "../Utility/Vector.hpp"
#include "../ECS/ECS.hpp"
#include "../ComponentSystem/StandardComponents.hpp"

namespace ECS
{
	class Box2DCollider final : public ComponentSystem
	{
	public:
		[[noreturn]] void initialize() noexcept override
		{
			if (!entity->hasComponent<Position>())
			{
				entity->addComponent<Position>();
			}
			pos = &entity->getComponent<Position>();
		}
		[[noreturn]] void update() noexcept override {}
		[[noreturn]] void draw2D() noexcept override {}
		[[noreturn]] void draw3D() noexcept override {}
	public:
		[[noreturn]] void offset(const Vector2& offset) noexcept
		{
			pos->position.offset(offset.xValue, offset.yValue, 0.0f);
		}
		[[nodiscard]] const Vector2& offsetCopy(const Vector2& offset) const noexcept
		{
			return pos->position.offsetCopy(offset.xValue, offset.yValue,0.0f);
		}
	private:
		Position* pos;
		Vector2 size;
	};
	class Box3DCollider final : public ComponentSystem
	{
	public:
		Box3DCollider() = default;
		virtual ~Box3DCollider() = default;
	public:
		[[noreturn]] void initialize() noexcept override
		{
			if (!entity->hasComponent<Position>())
			{
				entity->addComponent<Position>();
			}
			pos = &entity->getComponent<Position>();
		}
		[[noreturn]] void update() noexcept override {}
		[[noreturn]] void draw2D() noexcept override {}
		[[noreturn]] void draw3D() noexcept override {}
	public:
		[[noreturn]] void offset(const Vector3& offset) noexcept
		{
			pos->position.offset(offset.xValue, offset.yValue, offset.zValue);
		}
		[[nodiscard]] const Vector2& offsetCopy(const Vector3& offset) const noexcept
		{
			return pos->position.offsetCopy(offset.xValue, offset.yValue, offset.zValue);
		}
	private:
		Position* pos;
		Vector3 size;
	};
	class CircleCollider final : public ComponentSystem
	{
	public:
		[[noreturn]] void initialize() noexcept override
		{
			if (!entity->hasComponent<Position>())
			{
				entity->addComponent<Position>();
			}
			pos = &entity->getComponent<Position>();
		}
		[[noreturn]] void update() noexcept override {}
		[[noreturn]] void draw2D() noexcept override {}
		[[noreturn]] void draw3D() noexcept override {}
	private:
		Position* pos;
		float radius;
	};
	class SphereCollider final : public ComponentSystem
	{
	public:
		[[noreturn]] void initialize() noexcept override
		{
			if (!entity->hasComponent<Position>())
			{
				entity->addComponent<Position>();
			}
			pos = &entity->getComponent<Position>();
		}
		[[noreturn]] void update() noexcept override {}
		[[noreturn]] void draw2D() noexcept override {}
		[[noreturn]] void draw3D() noexcept override {}
	private:
		Position* pos;
		float radius;
	};
}