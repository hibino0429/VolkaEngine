//---------------------------------------------------------
//!@file ECS.hpp
//!@brief EntityComponentSystem�̎��Ԃł��B
//!@author Volka
//!@date 2018/11/3
//!@note �Q�l�� https ://github.com/SuperV1234/Tutorials
//---------------------------------------------------------
#pragma once
#include <bitset>
#include <array>
#include <memory>
#include <vector>
#include <assert.h>
#include <iostream>
#include <algorithm>
#include <memory>
#include <unordered_map>

namespace ECS
{
	class Entity;
	class ComponentSystem;
	class EntityManager;

	using ComponentID = std::size_t;
	using Group = std::size_t;


	inline ComponentID getNewComponentTypeID() noexcept
	{
		static ComponentID lastID = 0;
		return ++lastID;
	}

	template <typename T> inline ComponentID getComponentTypeID() noexcept
	{
		static ComponentID typeID = getNewComponentTypeID();
		return typeID;
	}

	constexpr std::size_t MaxComponents = 64;
	constexpr std::size_t MaxGroups = 32;

	using ComponentBitSet = std::bitset<MaxComponents>;
	using ComponentArray = std::array<ComponentSystem*, MaxComponents>;
	using GroupBitSet = std::bitset<MaxGroups>;

	class ComponentSystem
	{
	private:
		//Entity�ɂ���ĎE���ꂽ���̂ł����Ȃ���
		friend class Entity;
		bool active = true;
		void deleteThis() { active = false; }
	public:
		Entity * entity;
		virtual void initialize() = 0;
		virtual void update() = 0;
		virtual void draw3D() = 0;
		virtual void draw2D() = 0;
		virtual ~ComponentSystem() {}
		//���̃R���|�[�l���g�������Ă��邩�Ԃ��܂�
		virtual bool isActive() const final { return active; }

	};

	//�f�[�^�̓��\�b�h�������Ȃ�
	struct ComponentData : public ComponentSystem
	{
		void initialize() override final {}
		void update() override final {}
		void draw2D() override final {}
		void draw3D() override final {}
	};

	//1�ȏ�̃R���|�[�l���g�ɂ���Ē�`�����Entity
	class Entity final
	{
	private:
		friend class EntityManager;
		std::string tag;
		EntityManager& manager_;
		bool active = true;
		std::vector<std::unique_ptr<ComponentSystem>> components;
		ComponentArray  componentArray;
		ComponentBitSet componentBitSet;
		GroupBitSet groupBitSet;
		void refreshComponent()
		{
			components.erase(std::remove_if(std::begin(components), std::end(components),
				[](const std::unique_ptr<ComponentSystem> &pCom)
			{
				return !pCom->isActive();
			}),
				std::end(components));
		}
	public:
		Entity(EntityManager& manager) : manager_(manager) {}
		//!@brief ����Entity�ɂ��Ă���Component�̏������������s���܂�
		void initialize()
		{
			for (auto& c : components) c->initialize();
		}
		//!@brief ����Entity�ɂ��Ă���Component�̍X�V�������s���܂�
		void update()
		{
			refreshComponent();
			for (auto& c : components)
			{
				if (c == nullptr)
				{
					continue;
				}
				c->update();
			}
		}
		//!@brief ����Entity�ɂ��Ă���Component��3D�`�揈�����s���܂�
		void draw3D()
		{
			for (auto& c : components) c->draw3D();
		}
		//!@brief ����Entity�ɂ��Ă���Component��2D�`�揈�����s���܂�
		void draw2D()
		{
			for (auto& c : components)
			{
				if (c == nullptr)
				{
					continue;
				}
				c->draw2D();
			}
		}
		//!@brief Entity�̐�����Ԃ��擾���܂�
		bool isActive() const { return active; }
		//!@brief Entity���E���܂�
		void destroy()
		{
			active = false;
		}
		//!@brief Entity���w�肵���O���[�v�ɓo�^����Ă��邩�擾���܂�
		bool hasGroup(Group group) const noexcept
		{
			return groupBitSet[group];
		}
		//!@brief Entity���O���[�v�ɓo�^���܂�
		void addGroup(Group group) noexcept;
		//!@brief Entity���O���[�v��������܂�
		void deleteGroup(Group group) noexcept
		{
			groupBitSet[group] = false;
		}
		//!@brief Entity�Ɏw�肵��Component�����邩�擾���܂�
		template <typename T> bool hasComponent() const
		{
			return componentBitSet[getComponentTypeID<T>()];
		}

		//!@brief �R���|�[�l���g�̒ǉ����\�b�h
		//!@brief �ǉ����ꂽ��R���|�[�l���g�̏��������\�b�h���Ă΂�܂�
		template <typename T, typename... TArgs> T& addComponent(TArgs&&... args)
		{
			//�d���͋����Ȃ�
			if (hasComponent<T>())
			{
				return getComponent<T>();
			}
			//Tips: std::forward
			//�֐��e���v���[�g�̈�����]������B
			//���̊֐��́A�n���ꂽ������T&&�^�ɃL���X�g���ĕԂ��B�i���FT�����Ӓl�Q�Ƃ̏ꍇ�ɂ�T&&�����Ӓl�Q�ƂɂȂ�A����ȊO�̏ꍇ��T&&�͉E�Ӓl�Q�ƂɂȂ�B�j
			//���̊֐��́A��ɓ]���֐��iforwarding function�j�̎�����P��������ړI�Ŏg����F
			T* c(new T(std::forward<TArgs>(args)...));
			c->entity = this;
			std::unique_ptr<ComponentSystem> uPtr(c);
			components.emplace_back(std::move(uPtr));

			componentArray[getComponentTypeID<T>()] = c;
			componentBitSet[getComponentTypeID<T>()] = true;

			c->initialize();
			return *c;
		}

		//!@brief �w�肵���R���|�[�l���g���폜���܂�
		template<typename T> void removeComponent() noexcept
		{
			if (hasComponent<T>())
			{
				getComponent<T>().deleteThis();
				componentBitSet[getComponentTypeID<T>()] = false;
			}
		}

		//!@brief �o�^�����R���|�[�l���g���擾���܂�
		template<typename T> T& getComponent() const
		{
			if (!hasComponent<T>())
			{
				std::cout << typeid(T).name() << std::endl;
				assert(hasComponent<T>());
			}
			auto ptr(componentArray[getComponentTypeID<T>()]);
			return *static_cast<T*>(ptr);
		}
		//!@brief �^�O��Ԃ��܂�
		const std::string& getTag() const
		{
			return tag;
		}
	};

	//Entity�����N���X
	class EntityManager final
	{
	private:
		std::vector<std::unique_ptr<Entity>> entityes;
		std::array<std::vector<Entity*>, MaxGroups> groupedEntities;
	public:
		//!@brief ���������s���܂�
		void initialize()
		{
			for (auto& e : entityes)
			{
				e->initialize();
			}
		}
		//!@brief �X�V���s���܂�
		void update()
		{
			for (auto& e : entityes)
			{
				if (e == nullptr)
				{
					continue;
				}
				e->update();
			}
		}
		//!@brief 3D�̕`����s���܂�
		void draw3D()
		{
			for (auto& e : entityes)
			{
				e->draw3D();
			}
		}
		//!@brief �O���[�v���Ƃ̕`���o�^���ɍs��
		void orderByDraw(const size_t TheMaximumNumberOfRegistered)
		{
			for (auto i(0u); i < TheMaximumNumberOfRegistered; ++i)
			{
				const auto& entity = groupedEntities[i];
				for (const auto& e : entity)
				{
					e->draw2D();
				}
			}
		}
		//!@brief 2D�̕`����s���܂�
		void draw2D()
		{
			for (auto& e : entityes)
			{
				e->draw2D();
			}
		}
		//!@brief ���ׂĂ�Entity��Kill���܂�
		void allKill()
		{
			for (auto& e : entityes)
			{
				e->destroy();
			}
		}
		//!@brief �A�N�e�B�u�łȂ����̂��폜���܂�
		void refresh()
		{
			for (auto i(0u); i < MaxGroups; ++i)
			{
				auto& v(groupedEntities[i]);

				v.erase(std::remove_if(std::begin(v), std::end(v),
					[i](Entity* pEntity)
				{
					return !pEntity->isActive() ||
						!pEntity->hasGroup(i);
				}),
					std::end(v));
			}

			entityes.erase(std::remove_if(std::begin(entityes), std::end(entityes),
				[](const std::unique_ptr<Entity> &pEntity)
			{
				return !pEntity->isActive();
			}),
				std::end(entityes));
		}
		//!@brief �w�肵���O���[�v�ɓo�^����Ă���Entity�B���擾���܂�
		std::vector<Entity*>& getEntitiesByGroup(Group group)
		{
			return groupedEntities[group];
		}
		//!@brief Entity���w�肵���O���[�v�ɓo�^���܂�
		void addToGroup(Entity* pEntity, Group group)
		{
			groupedEntities[group].emplace_back(pEntity);
		}
		//!@brief Entity�𐶐������̃|�C���^��Ԃ�
		//!@brief �^�O��ݒ肵�Ă����ƃf�o�b�O����Ƃ��ɒǂ������₷��
		Entity& addEntityAddTag(const std::string& tag)
		{
			Entity* e = new Entity(*this);
			std::unique_ptr<Entity> uPtr(e);
			entityes.emplace_back(std::move(uPtr));
			entityes.back()->tag = tag;
			return *e;
		}
		//!@brief Entity�𐶐������̃|�C���^��Ԃ�
		//!@brief ��{�I�ɂ�������g��
		Entity& addEntity()
		{
			Entity* e = new Entity(*this);
			std::unique_ptr<Entity> uPtr(e);
			entityes.emplace_back(std::move(uPtr));
			entityes.back()->tag = "";
			return *e;
		}
		//�^�O���w�肵����Entity���擾����
		//���s�����ꍇ������
		//Entity& GetEntity(const std::string& tag)
		//{
		//	constexpr bool  NOT_FOUND_TAG = false;
		//	assert(!(tag == ""));
		//	for (const auto& it : entityes)
		//	{
		//		if (it->tag == tag)
		//		{
		//			return *it;
		//		}
		//	}
		//	//�擾�Ɏ��s�����ꍇ�͂Ƃ肠�������Ƃ�
		//	assert(NOT_FOUND_TAG);
		//	return *entityes[0];
		//}
	};

	//!@class Entity�̌��^����邽�߂̃C���^�[�t�F�[�X
	template<class... Args>
	class IArcheType
	{
	private:
		virtual ECS::Entity* operator()(Args...) = 0;
	};

	//!@class EntitySystemManager
	//!@brief EntityManager���Ǘ�����V�X�e��
	class EntitySystemManager final
	{
		class Singleton final
		{
		public:
			virtual ~Singleton() noexcept final
			{
				manager.clear();
			}
			//!@brief EntityManager��o�^���܂�
			[[noreturn]] void regist(const std::string& name, std::unique_ptr<ECS::EntityManager>& entityManager) noexcept
			{
				manager[name] = std::move(entityManager);
			}
			//!@brief EntityManager���폜���܂�
			[[noreturn]] void remove(const std::string& name) noexcept
			{
				const auto& itr = manager.find(name);
				if (itr != manager.end())
				{
					manager.erase(itr);
				}
			}
			//!@brief EntityManager���擾���܂�
			[[nodiscard]] ECS::EntityManager& getEntityManager(const std::string& name) const noexcept
			{
				return *manager.at(name).get();
			}
			//!@brief EntityManager�����邩�m�F����
			[[nodiscard]] const bool hasEntityManager(const std::string& name) const noexcept
			{
				const auto& itr = manager.find(name);
				if (itr == manager.end())
				{
					return false;
				}
				return true;
			}
			[[noreturn]] void initialize() noexcept
			{
				for (const auto& m : manager)
				{
					m.second.get()->initialize();
				}
			}
			[[noreturn]] void update() noexcept
			{
				for (const auto& m : manager)
				{
					m.second.get()->refresh();
					m.second.get()->update();
					m.second.get()->draw2D();
					m.second.get()->draw3D();
				}
			}
		private:
			std::unordered_map<std::string, std::unique_ptr<ECS::EntityManager>> manager;
		};
	public:
		[[nodiscard]] inline static Singleton& get() noexcept
		{
			static std::unique_ptr<Singleton> systemManager
				= std::make_unique<Singleton>();
			return *systemManager;
		}
	};
}
