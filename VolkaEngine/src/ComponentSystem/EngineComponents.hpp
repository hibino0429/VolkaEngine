//---------------------------------------------------------
//!@file EngineComponents.hpp
//!@brief 標準的なエンジン側のComponent群を用意します
//!@author Volka
//!@date 2018/11/3
//!@note window, renderer など
//---------------------------------------------------------
#pragma once
#include <SDL.h>
#include <iostream>
#include <typeinfo>
#include <SDL2_image/SDL_image.h>
#include "../ComponentSystem/StandardComponents.hpp"

#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")
#pragma comment(lib, "SDL2_image.lib")


/** NOTE: Windows on KVM cannot call direct3D.
	Then SDL_RENDERER_SOFTWARE will be needed. */
#ifdef NEED_RENDERER_SOFTWARE
#define SDL_RENDERER_FLAGS (SDL_RENDERER_SOFTWARE)
#else
#define SDL_RENDERER_FLAGS (0)
#endif

#define ENTITY_GROUP (ECS::Group)ECS::EntityGroup

namespace ECS
{
	enum class EntityGroup : ECS::Group
	{
		Engine,
	};
	//!@class WindowSystem
	class WindowSystem final : public ECS::ComponentSystem
	{
	public:
		virtual ~WindowSystem()
		{
			SDL_DestroyWindow(window);
			SDL_Quit();
		}
		[[noreturn]] void initialize() noexcept override
		{
			if (!entity->hasComponent<ECS::Screen>())
			{
				entity->addComponent<ECS::Screen>();
			}
			screen = &entity->getComponent<ECS::Screen>();
			if (!entity->hasComponent<ECS::Color>())
			{
				entity->addComponent<ECS::Color>((Uint8)0x00, (Uint8)0xFF, (Uint8)0xFF, (Uint8)0xFF);
			}
			color = &entity->getComponent<ECS::Color>();
			if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
				//SDL_PrintError(SDL_Init);
			}
			createWindow();
		}
		[[noreturn]] void update() noexcept override {}
		[[noreturn]] void draw2D() noexcept override {}
		[[noreturn]] void draw3D() noexcept override {}
	public:
		//!@brief windowの取得
		[[nodiscard]] SDL_Window& GetWindow() const noexcept
		{
			return *window;
		}
		//!@brief surfaceの取得
		[[nodiscard]] SDL_Surface& GetSurface() const noexcept
		{
			return *surface;
		}
	private:
		[[noreturn]] void createWindow() noexcept
		{
			window = SDL_CreateWindow(screen->screenName.c_str(),
				SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
				static_cast<int>(screen->size.xValue), static_cast<int>(screen->size.yValue),
				SDL_WINDOW_SHOWN);
			if (window == nullptr)
			{
				//SDL_PrintError(SDL_CreateWindow);
				SDL_DestroyWindow(window);
			}
			else
			{
				//get window surface
				surface = SDL_GetWindowSurface(window);
				//fill surface color
				SDL_FillRect(surface, nullptr, SDL_MapRGB(surface->format,
					color->value.xValue, color->value.yValue, color->value.zValue));
				//update surface
				SDL_UpdateWindowSurface(window);
			}
		}
	private:
		SDL_Window* window;
		SDL_Surface* surface;
		ECS::Screen* screen;
		ECS::Color* color;
	};


	//!@class RendererSystem
	class RendererSystem final : public ECS::ComponentSystem
	{
	public:
		virtual ~RendererSystem()
		{
			SDL_DestroyRenderer(renderer);
		}
		[[noreturn]] void initialize() noexcept override
		{
			window = &entity->getComponent<WindowSystem>();
			color = &entity->getComponent<Color>();
			renderer = SDL_CreateRenderer(&window->GetWindow(), -1, SDL_RENDERER_FLAGS);
			if (renderer == nullptr)
			{
				//SDL_PrintError(SDL_CreateRenderer);
				SDL_Quit();
			}
			SDL_SetRenderDrawColor(renderer, color->value.xValue, color->value.yValue, color->value.zValue, color->value.wValue);
		}
		[[noreturn]] void update() noexcept override {}
		[[noreturn]] void draw2D() noexcept override {}
		[[noreturn]] void draw3D() noexcept override {}
	public:
		[[nodiscard]] SDL_Renderer& GetRenderer() const noexcept
		{
			return *renderer;
		}
	private:
		SDL_Renderer* renderer;
		WindowSystem* window;
		Color* color;
	};

	template<typename KeyType, typename ValueType>
	class Asset : public ECS::ComponentSystem
	{
	public:
		using KeyTypeData = KeyType;
		using ValueTypeData = ValueType;
	public:
		[[noreturn]] void initialize() noexcept override {}
		[[noreturn]] void update() noexcept override {}
		[[noreturn]] void draw2D() noexcept override {}
		[[noreturn]] void draw3D() noexcept override {}
		//!@brief [key]に対応した[value]の取得
		[[nodiscard]] const ValueType& getTexture(const KeyType& key) const noexcept
		{
			return asset.at(key);
		}
		//!@brief [key]と[value]を登録
		[[noreturn]] void regist(const KeyType& key, const ValueType& value) noexcept
		{
			asset[key] = value;
		}
		//!@brief [key]の[value]の削除
		[[nodiscard]] void remove(const KeyType& key) noexcept
		{
			auto& itr = asset.find(key);
			if (itr != asset.end())
			{
				asset.erase(itr);
			}
		}
		//!@brief map全体の取得
		[[nodiscard]] const std::unordered_map<KeyType, ValueType>& get() const noexcept
		{
			return asset;
		}
	private:
		std::unordered_map<KeyType, ValueType> asset;
	};

	//デフォルト texture
	using AssetTexture = Asset<std::string, SDL_Texture*>;
	//デフォルト sound
}