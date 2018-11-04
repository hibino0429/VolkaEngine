//---------------------------------------------------------
//!@file ECS.hpp
//!@brief EntityComponentSystemの実態です。
//!@author Volka
//!@date 2018/11/3
//!@note 参考元 https ://github.com/SuperV1234/Tutorials
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
		//Entityによって殺されたいのでこうなった
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
		//このコンポーネントが生きているか返します
		virtual bool isActive() const final { return active; }

	};

	//データはメソッドを持たない
	struct ComponentData : public ComponentSystem
	{
		void initialize() override final {}
		void update() override final {}
		void draw2D() override final {}
		void draw3D() override final {}
	};

	//1つ以上のコンポーネントによって定義されるEntity
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
		//!@brief このEntityについているComponentの初期化処理を行います
		void initialize()
		{
			for (auto& c : components) c->initialize();
		}
		//!@brief このEntityについているComponentの更新処理を行います
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
		//!@brief このEntityについているComponentの3D描画処理を行います
		void draw3D()
		{
			for (auto& c : components) c->draw3D();
		}
		//!@brief このEntityについているComponentの2D描画処理を行います
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
		//!@brief Entityの生存状態を取得します
		bool isActive() const { return active; }
		//!@brief Entityを殺します
		void destroy()
		{
			active = false;
		}
		//!@brief Entityが指定したグループに登録されているか取得します
		bool hasGroup(Group group) const noexcept
		{
			return groupBitSet[group];
		}
		//!@brief Entityをグループに登録します
		void addGroup(Group group) noexcept;
		//!@brief Entityをグループから消します
		void deleteGroup(Group group) noexcept
		{
			groupBitSet[group] = false;
		}
		//!@brief Entityに指定したComponentがあるか取得します
		template <typename T> bool hasComponent() const
		{
			return componentBitSet[getComponentTypeID<T>()];
		}

		//!@brief コンポーネントの追加メソッド
		//!@brief 追加されたらコンポーネントの初期化メソッドが呼ばれます
		template <typename T, typename... TArgs> T& addComponent(TArgs&&... args)
		{
			//重複は許可しない
			if (hasComponent<T>())
			{
				return getComponent<T>();
			}
			//Tips: std::forward
			//関数テンプレートの引数を転送する。
			//この関数は、渡された引数をT&&型にキャストして返す。（注：Tが左辺値参照の場合にはT&&も左辺値参照になり、それ以外の場合にT&&は右辺値参照になる。）
			//この関数は、主に転送関数（forwarding function）の実装を単純化する目的で使われる：
			T* c(new T(std::forward<TArgs>(args)...));
			c->entity = this;
			std::unique_ptr<ComponentSystem> uPtr(c);
			components.emplace_back(std::move(uPtr));

			componentArray[getComponentTypeID<T>()] = c;
			componentBitSet[getComponentTypeID<T>()] = true;

			c->initialize();
			return *c;
		}

		//!@brief 指定したコンポーネントを削除します
		template<typename T> void removeComponent() noexcept
		{
			if (hasComponent<T>())
			{
				getComponent<T>().deleteThis();
				componentBitSet[getComponentTypeID<T>()] = false;
			}
		}

		//!@brief 登録したコンポーネントを取得します
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
		//!@brief タグを返します
		const std::string& getTag() const
		{
			return tag;
		}
	};

	//Entity統括クラス
	class EntityManager final
	{
	private:
		std::vector<std::unique_ptr<Entity>> entityes;
		std::array<std::vector<Entity*>, MaxGroups> groupedEntities;
	public:
		//!@brief 初期化を行います
		void initialize()
		{
			for (auto& e : entityes)
			{
				e->initialize();
			}
		}
		//!@brief 更新を行います
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
		//!@brief 3Dの描画を行います
		void draw3D()
		{
			for (auto& e : entityes)
			{
				e->draw3D();
			}
		}
		//!@brief グループごとの描画を登録順に行う
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
		//!@brief 2Dの描画を行います
		void draw2D()
		{
			for (auto& e : entityes)
			{
				e->draw2D();
			}
		}
		//!@brief すべてのEntityをKillします
		void allKill()
		{
			for (auto& e : entityes)
			{
				e->destroy();
			}
		}
		//!@brief アクティブでないものを削除します
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
		//!@brief 指定したグループに登録されているEntity達を取得します
		std::vector<Entity*>& getEntitiesByGroup(Group group)
		{
			return groupedEntities[group];
		}
		//!@brief Entityを指定したグループに登録します
		void addToGroup(Entity* pEntity, Group group)
		{
			groupedEntities[group].emplace_back(pEntity);
		}
		//!@brief Entityを生成しそのポインタを返す
		//!@brief タグを設定しておくとデバッグするときに追いかけやすい
		Entity& addEntityAddTag(const std::string& tag)
		{
			Entity* e = new Entity(*this);
			std::unique_ptr<Entity> uPtr(e);
			entityes.emplace_back(std::move(uPtr));
			entityes.back()->tag = tag;
			return *e;
		}
		//!@brief Entityを生成しそのポインタを返す
		//!@brief 基本的にこちらを使う
		Entity& addEntity()
		{
			Entity* e = new Entity(*this);
			std::unique_ptr<Entity> uPtr(e);
			entityes.emplace_back(std::move(uPtr));
			entityes.back()->tag = "";
			return *e;
		}
		//タグを指定しそのEntityを取得する
		//失敗した場合落ちる
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
		//	//取得に失敗した場合はとりあえず落とす
		//	assert(NOT_FOUND_TAG);
		//	return *entityes[0];
		//}
	};

	//!@class Entityの原型を作るためのインターフェース
	template<class... Args>
	class IArcheType
	{
	private:
		virtual ECS::Entity* operator()(Args...) = 0;
	};

	//!@class EntitySystemManager
	//!@brief EntityManagerを管理するシステム
	class EntitySystemManager final
	{
		class Singleton final
		{
		public:
			virtual ~Singleton() noexcept final
			{
				manager.clear();
			}
			//!@brief EntityManagerを登録します
			[[noreturn]] void regist(const std::string& name, std::unique_ptr<ECS::EntityManager>& entityManager) noexcept
			{
				manager[name] = std::move(entityManager);
			}
			//!@brief EntityManagerを削除します
			[[noreturn]] void remove(const std::string& name) noexcept
			{
				const auto& itr = manager.find(name);
				if (itr != manager.end())
				{
					manager.erase(itr);
				}
			}
			//!@brief EntityManagerを取得します
			[[nodiscard]] ECS::EntityManager& getEntityManager(const std::string& name) const noexcept
			{
				return *manager.at(name).get();
			}
			//!@brief EntityManagerがあるか確認する
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
