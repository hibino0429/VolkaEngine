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

	//!@class Color
	//!@brief 色を扱います
	template<typename ColorType = Uint8>
	struct Color final : public ComponentData
	{
		explicit Color() noexcept
			: value(Vector4Type<ColorType>(0xFF, 0xFF, 0xFF, 0xFF))
		{}
		explicit Color(const ColorType r, const ColorType g, const ColorType b, const ColorType a = 0xFF) noexcept
			: value(r, g, b, a)
		{}
		//!@brief 色の設定
		[[noreturn]] void setColor(const Vector4Type<ColorType>& color) noexcept
		{
			value = color;
		}
		Vector4Type<ColorType> value;
	};
	//!@class Counter
	//!@brief カウントを行います
	class Counter final : public ComponentSystem
	{
	public:
		explicit Counter()
			: MaxCount(0.0f)
		{}
		explicit Counter(const float maxCount)
			: MaxCount(maxCount)
		{}
		[[noreturn]] void initialize() noexcept override
		{
			cnt = 0.0f;
		}
		[[noreturn]] void update() noexcept override
		{
			if (isMaxCount())
			{
				cnt = 0.0f;
			}
			++cnt;
		}
		[[noreturn]] void draw2D() noexcept override {}
		[[noreturn]] void draw3D() noexcept override {}
		//!@brief 最大のカウントかどうか
		const bool isMaxCount() const noexcept
		{
			return cnt >= MaxCount;
		}
		//!@brief リセットします
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
				entity->addComponent<Counter>();
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

	//!@class Screen
	//!@brief スクリーンを扱います
	class Screen final : public ComponentData
	{
	public:
		explicit Screen() noexcept
			: screenName("title"), pos(Vector2(0, 0)), size(Vector2(0, 0))
		{}
		explicit Screen(const std::string& screenTitle,const Vector2& screenSize) noexcept
			: screenName(screenTitle),pos(Vector2(0,0)),size(screenSize)
		{}
		explicit Screen(const std::string& screenTitle,const Vector2& screenPos, const Vector2& screenSize) noexcept
			: screenName(screenTitle),pos(screenPos), size(screenSize)
		{}
		//!@brief スクリーン名の設定
		[[noreturn]] void setScreenName(const std::string& screenTitle) noexcept
		{
			screenName = screenTitle;
		}
		//!@brief スクリーンのサイズの設定
		[[noreturn]] void setScreenSize(const Vector2& screenSize) noexcept
		{
			size = screenSize;
		}
	public:
		std::string screenName;
		Vector2 pos;
		Vector2 size;
	};

}