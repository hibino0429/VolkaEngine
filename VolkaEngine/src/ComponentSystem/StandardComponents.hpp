//---------------------------------------------------------
//!@file StandardComponents.hpp
//!@brief 標準的なComponent群を用意します
//!@author OxthLy
//!@date 2018/11/3
//!@note Position,Rotation,Scale,Transform など
//!@note 参考URL https://github.com/tonarinohito/MyGameTemplate/blob/master/gameTemple/src/Components/BasicComponents.hpp
//!@note by tonarinohito
//---------------------------------------------------------
#pragma once
#include "../ECS/ECS.hpp"
#include "../Utility/Vector.hpp"


namespace ECS
{
	//!@class Position
	//!@brief 座標を扱います
	struct Position final : public ComponentData
	{
		Position() = default;
		explicit Position(const Vector3& vector3) noexcept
			: position(vector3)
		{}
		explicit Position(const float& x, const float& y, const float& z) noexcept
			: position(x, y, z)
		{}
		Vector3 position;
	};

	//!@class Rotation
	//!@brief 回転を扱います
	struct Rotation final : public ComponentData
	{
		Rotation() = default;
		explicit Rotation(const Vector3& vector3) noexcept
			: rotation(vector3)
		{}
		explicit Rotation(const float& rotate) noexcept
			: rotation(rotate, rotate, rotate)
		{}
		Vector3 rotation;
	};

	//!@class Scale
	//!@brief 拡大縮小を扱います
	struct Scale final : public ComponentData
	{
		Scale() = default;
		explicit Scale(const Vector3& vector3) noexcept
			: scale(vector3)
		{}
		explicit Scale(const float& scaler) noexcept
			: scale(scaler, scaler, scaler)
		{}
		Vector3 scale;
	};

	//!@class Transform
	//!@brief 座標・回転・拡大縮小の変換を扱います
	class Transform final : public ComponentSystem
	{
	public:
		explicit Transform() noexcept
			: pos(nullptr), rot(nullptr), sca(nullptr)
		{}
		[[noreturn]] void initialize() noexcept override
		{
			if (!entity->hasComponent<Position>())
			{
				entity->addComponent<Position>();
			}
			if (!entity->hasComponent<Rotation>())
			{
				entity->addComponent<Rotation>();
			}
			if (!entity->hasComponent<Scale>())
			{
				entity->addComponent<Scale>();
			}
			pos = &entity->getComponent<Position>();
			rot = &entity->getComponent<Rotation>();
			sca = &entity->getComponent<Scale>();
		}
	private:
		Position* pos;
		Rotation* rot;
		Scale* sca;
	};

	//!@class Counter
	//!@brief カウントを行います
	class Counter final : public ComponentSystem
	{
	public:
		explicit Counter()
			: cnt(0.0f) , MaxCount(0.0f)
		{}
		explicit Counter(const float maxCount)
			: MaxCount(maxCount)
		{}
		[[noreturn]] void update() noexcept override
		{
			if (isMaxCount())
			{
				cnt = 0.0f;
			}
			++cnt;
		}
		const bool isMaxCount() const noexcept
		{
			return cnt >= MaxCount;
		}
		[[noreturn]] void reset() noexcept
		{
			cnt = 0.0f;
		}
	private:
		float cnt;
		const float MaxCount;
	};

	//!@class KillEntity
	//!@brief アタッチしたEntityを指定時間で削除します
	class KillEntity final : public ComponentSystem
	{
	public:
		explicit KillEntity()
			: maxCountSpan(0.0f)
		{}
		explicit KillEntity(const float countSpan)
			: maxCountSpan(countSpan)
		{}
		[[noreturn]] void initialize() noexcept override
		{
			if (!entity->hasComponent<Counter>())
			{
				entity->addComponent<Counter>(maxCountSpan);
			}
			counter = &entity->getComponent<Counter>();
		}
		[[noreturn]] void update() noexcept override
		{
			if (counter->isMaxCount())
			{
				entity->destroy();
			}
		}
		[[noreturn]] void setKillLimitTime(const float maxCount) noexcept
		{
			maxCountSpan = maxCount;
		}
	private:
		Counter* counter;
		float maxCountSpan;
	};

	//!@class Direction
	//!@brief 向きを扱います
	struct Direction final : public ComponentData
	{
		enum class DirState : short
		{
			XForward,  //x+
			XBack,     //x-
			YForward,  //y+
			YBack,     //y-
			ZForward,  //z+
			ZBack,     //z-
		};
		explicit Direction()
			: value(DirState::XForward)
		{}
		DirState value;
	};

	//!@class Gravity
	//!@brief 重力を扱います
	struct Gravity final : public ComponentData
	{
		explicit Gravity(const float gravity = DefaultGravity)
			: value(gravity)
		{}
		static constexpr float DefaultGravity = 9.8f / 60.0f / 60.0f * 32.0f * 3.0f;
		float value;
	};
}