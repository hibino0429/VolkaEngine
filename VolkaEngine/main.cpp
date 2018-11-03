#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")
#include <SDL.h>
#include <iostream>
#include "src/ECS/ECS.hpp"
#include "src/ComponentSystem/StandardComponents.hpp"



//#pragma comment(lib, "SDL2_image.lib")
//#pragma comment(lib, "SDL2_mixer.lib")
//#pragma comment(lib, "SDL2_ttf.lib")



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
		if (!entity->hasComponent<ECS::Screen>())
		{
			entity->addComponent<ECS::Screen>();
		}
		screen = &entity->getComponent<ECS::Screen>();
		if (!entity->hasComponent<ECS::Color<Uint8>>())
		{
			entity->addComponent<ECS::Color<Uint8>>(0x00, 0xFF, 0xFF);
		}
		color = &entity->getComponent<ECS::Color<>>();

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
		window = SDL_CreateWindow(screen->screenName.c_str(), 
			SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED, 
			static_cast<int>(screen->size.xValue),static_cast<int>(screen->size.yValue), SDL_WINDOW_SHOWN);
		if (window == nullptr) {
			SDL_PrintError(SDL_CreateWindow);
			isSuccessCreateWindow = false;
		}
		else
		{
			isSuccessCreateWindow = true;
			//get window surface
			surface = SDL_GetWindowSurface(window);
			//fill surface white
			SDL_FillRect(surface, nullptr, SDL_MapRGB(surface->format, 
				color->value.xValue, color->value.yValue, color->value.zValue));
			//update surface
			SDL_UpdateWindowSurface(window);
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
	SDL_Surface* surface;
	ECS::Screen* screen;
	ECS::Color<>* color;
};



int main(int /*argc*/, char** /*argv*/)
{
	ECS::EntitySystemManager::get().initialize();

	std::unique_ptr<ECS::EntityManager> deviceManager = std::make_unique<ECS::EntityManager>();
	ECS::Entity* deviceEntity = &deviceManager.get()->addEntity();
	deviceEntity->addComponent<ECS::Color<>>(0x00,0xFF,0xFF);
	deviceEntity->addComponent<ECS::Screen>("window", Vector2(1080, 720));
	deviceEntity->addComponent<SDLSystem>();
	ECS::EntitySystemManager::get().regist("device", deviceManager.get());

	while(1)
	{
		ECS::EntitySystemManager::get().update();
	}
	return 0;
}



