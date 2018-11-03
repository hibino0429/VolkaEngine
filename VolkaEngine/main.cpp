#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")
#include <SDL.h>
#include <iostream>
#include "src/ECS/ECS.hpp"
#include <memory>
#include <map>
#include <unordered_map>



//#pragma comment(lib, "SDL2_image.lib")
//#pragma comment(lib, "SDL2_mixer.lib")
//#pragma comment(lib, "SDL2_ttf.lib")



//int main(int argc, char ** args) {
//
//	std::cout << SDL_GetError() << std::endl;
//
//	SDL_CreateWindow("title", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 500, 500, 1);
//	SDL_Init(SDL_INIT_EVERYTHING);
//
//	SDL_Delay(3000);
//
//	SDL_Quit();
//
//	system("pause");
//	return 0;
//}

#define SDL_WINDOW_TITLE "SDL2"
#define SDL_WINDOW_WIDTH (640)
#define SDL_WINDOW_HEIGHT (360)

/** NOTE: Windows on KVM cannot call direct3D.
	Then SDL_RENDERER_SOFTWARE will be needed. */
#ifdef NEED_RENDERER_SOFTWARE
#define SDL_RENDERER_FLAGS (SDL_RENDERER_SOFTWARE)
#else
#define SDL_RENDERER_FLAGS (0)
#endif

#define SDL_PrintError(name)                                    \
  do {                                                          \
    std::cerr << #name << ": " << SDL_GetError() << std::endl;  \
  } while (0)


class SDLSystem final: public ECS::ComponentSystem
{
public:
	SDLSystem()
		: window(nullptr), renderer(nullptr)
	{}
	virtual ~SDLSystem() noexcept final
	{
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
	}
	[[noreturn]] void initialize() noexcept override
	{
		if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
			SDL_PrintError(SDL_Init);
		}
		createWindow();
		createRenderer();
	}
	[[noreturn]] void update() noexcept override {}
	[[noreturn]] void draw2D() noexcept override {}
	[[noreturn]] void draw3D() noexcept override {}
private:
	[[noreturn]] void createWindow() noexcept
	{
		window = SDL_CreateWindow(SDL_WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOW_WIDTH,
			SDL_WINDOW_HEIGHT, 0);
		if (window == nullptr) {
			SDL_PrintError(SDL_CreateWindow);
			isSuccessCreateWindow = false;
		}
		else
		{
			isSuccessCreateWindow = true;
		}
		if (!isSuccessCreateWindow)
		{
			SDL_DestroyWindow(window);
		}
	}
	[[noreturn]] void createRenderer() noexcept
	{
		renderer = SDL_CreateRenderer(window,-1, SDL_RENDERER_FLAGS);
		if (renderer == nullptr) {
			SDL_PrintError(SDL_CreateRenderer);
			isSuccessCreateRenderer = false;
		}
		else
		{
			isSuccessCreateRenderer = true;
		}
		if (!isSuccessCreateRenderer)
		{
			SDL_Quit();
		}
	}
private:
	bool isSuccessCreateWindow;
	bool isSuccessCreateRenderer;
	SDL_Window* window;
	SDL_Renderer* renderer;
};



//EntityManagerを管理するシステムです
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
		[[noreturn]] void regist(const std::string& name, ECS::EntityManager* entityManager) noexcept
		{
			std::unique_ptr<ECS::EntityManager> eManager(entityManager);
			manager[name] = std::move(eManager);
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
		[[nodiscard]] const ECS::EntityManager& getEntityManager(const std::string& name) const noexcept
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

int main(int /*argc*/, char** /*argv*/)
{
	EntitySystemManager::get().initialize();

	std::unique_ptr<ECS::EntityManager> deviceManager = std::make_unique<ECS::EntityManager>();
	ECS::Entity* deviceEntity = &deviceManager.get()->addEntity();
	deviceEntity->addComponent<SDLSystem>();
	EntitySystemManager::get().regist("device", deviceManager.get());

	while(1)
	{
		EntitySystemManager::get().update();
	}
	return 0;
}



