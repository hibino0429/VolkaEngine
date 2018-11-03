//---------------------------------------------------------
//!@file StandardComponents.hpp
//!@brief �W���I��Component�Q��p�ӂ��܂�
//!@author OxthLy
//!@date 2018/11/3
//!@note Position,Rotation,Scale,Transform �Ȃ�
//!@note �Q�lURL https://github.com/tonarinohito/MyGameTemplate/blob/master/gameTemple/src/Components/BasicComponents.hpp
//!@note by tonarinohito
//---------------------------------------------------------
#pragma once
#include "../ECS/ECS.hpp"
#include "../Utility/Vector.hpp"


namespace ECS
{
	//!@class Position
	//!@brief ���W�������܂�
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
	//!@brief ��]�������܂�
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
	//!@brief �g��k���������܂�
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
	//!@brief ���W�E��]�E�g��k���̕ϊ��������܂�
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
	//!@brief �F�������܂�
	template<typename ColorType = Uint8>
	struct Color final : public ComponentData
	{
		explicit Color() noexcept
			: value(Vector4Type<ColorType>(0xFF, 0xFF, 0xFF, 0xFF))
		{}
		explicit Color(const ColorType r, const ColorType g, const ColorType b, const ColorType a = 0xFF) noexcept
			: value(r, g, b, a)
		{}
		//!@brief �F�̐ݒ�
		[[noreturn]] void setColor(const Vector4Type<ColorType>& color) noexcept
		{
			value = color;
		}
		Vector4Type<ColorType> value;
	};
	//!@class Counter
	//!@brief �J�E���g���s���܂�
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
		//!@brief �ő�̃J�E���g���ǂ���
		const bool isMaxCount() const noexcept
		{
			return cnt >= MaxCount;
		}
		//!@brief ���Z�b�g���܂�
		[[noreturn]] void reset() noexcept
		{
			cnt = 0.0f;
		}
	private:
		float cnt;
		const float MaxCount;
	};

	//!@class KillEntity
	//!@brief �A�^�b�`����Entity���w�莞�Ԃō폜���܂�
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
	//!@brief �����������܂�
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
	//!@brief �d�͂������܂�
	struct Gravity final : public ComponentData
	{
		explicit Gravity(const float gravity = DefaultGravity)
			: value(gravity)
		{}
		static constexpr float DefaultGravity = 9.8f / 60.0f / 60.0f * 32.0f * 3.0f;
		float value;
	};

	//!@class Screen
	//!@brief �X�N���[���������܂�
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
		//!@brief �X�N���[�����̐ݒ�
		[[noreturn]] void setScreenName(const std::string& screenTitle) noexcept
		{
			screenName = screenTitle;
		}
		//!@brief �X�N���[���̃T�C�Y�̐ݒ�
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