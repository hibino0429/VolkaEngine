#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")
#include <SDL.h>
#include <iostream>

#include <SDL2_image/SDL_image.h>
#include "src/ECS/ECS.hpp"
#include "src/ComponentSystem/StandardComponents.hpp"



#pragma comment(lib, "SDL2_image.lib")
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



//!@class SDLSystem
//!@brief SDLの初期化、終了処理を行います
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
	[[noreturn]] void update() noexcept override
	{
		SDL_UpdateWindowSurface(window);
		surface = SDL_GetWindowSurface(window);
	}
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
			//fill surface color
			SDL_FillRect(surface, nullptr, SDL_MapRGB(surface->format,
				color->value.xValue, color->value.yValue, color->value.zValue));
			//update surface
			SDL_UpdateWindowSurface(window);
			surface = SDL_GetWindowSurface(window);
		}
		if (!isSuccessCreateWindow)
		{
			SDL_DestroyWindow(window);
		}
	}
	[[noreturn]] void createRenderer() noexcept
	{
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_FLAGS);
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
public:
	[[nodiscard]] SDL_Surface& GetSurface() const noexcept
	{
		return *surface;
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


//複数のアセットを管理し、マネージャーの役割も持つ
template<typename KeyType, typename ValueType>
class AssetManager
{
public:
	class Singleton
	{
	public:
		void regist(const KeyType& key, const ValueType& value)
		{
			assetList[key] = value;
		}
		void remove(const KeyType& key)
		{
			auto& itr = assetList.find();
			if (itr != assetList.end())
			{
				assetList.erase(itr);
			}
		}
	private:
		std::unordered_map<KeyType, ValueType> assetList;
	};
	inline static const Singleton& Get()
	{
		static std::unique_ptr<Singleton> asset
			= std::make_unique<Singleton>();
		return *asset;
	}
};


class LoadTexture final : public ECS::ComponentSystem
{
public:
	LoadTexture()
		: path("")
	{}
	LoadTexture(const std::string& filePath)
	{
		path = filePath;
	}
	virtual ~LoadTexture()
	{
		SDL_FreeSurface(textureSurface);
		textureSurface = nullptr;
		IMG_Quit();
	}
	[[noreturn]] void update() noexcept override
	{
		SDL_Surface* screen = &sdlSystem->GetSurface();
		SDL_BlitSurface(textureSurface, nullptr, screen, nullptr);
		
	}
	[[noreturn]] void initialize() noexcept override
	{
		if (!entity->hasComponent<SDLSystem>())
		{
			entity->addComponent<SDLSystem>();
		}
		sdlSystem = &entity->getComponent<SDLSystem>();
		
		int imgFlags = IMG_INIT_PNG;
		int initted = IMG_Init(imgFlags);
		//initialize png loading
		if (initted & imgFlags != imgFlags)
		{
			SDL_PrintError(IMG_GetError);
		}
		else
		{
			textureSurface = LoadSurface(path);
		}
	}
	[[noreturn]] void draw2D() noexcept override {}
	[[noreturn]] void draw3D() noexcept override {}
private:
	//!@brief 画像のサーフェイスの作成
	[[nodiscard]] SDL_Surface* LoadSurface(const std::string& path) const noexcept
	{
		//final optimized image
		SDL_Surface* optimizedSurface = nullptr;
		//load image path
		SDL_Surface* loadSurface = IMG_Load(path.c_str());
		if (loadSurface == nullptr)
		{
			SDL_PrintError(loadSurface);
		}
		else
		{
			SDL_Surface* surface = &sdlSystem->GetSurface();
			//convert surface to screen format
			optimizedSurface = SDL_ConvertSurface(loadSurface, surface->format, 0);
			if (optimizedSurface == nullptr)
			{
				SDL_PrintError(optimizedSurface);
			}
			else
			{
				//old load surface free
				SDL_FreeSurface(loadSurface);
			}
		}
		return optimizedSurface;
	}
	//!@brief 画像形式が対応しているかチェック
	//!@return true:対応  false:対応していない
	[[nodiscard]] const bool CheckImageFormat() const noexcept
	{
		SDL_RWops* rwop = SDL_RWFromFile(path.c_str(), "rb");
		if (IMG_isPNG(rwop) || IMG_isBMP(rwop) || IMG_isJPG(rwop) ||
			IMG_isTIF(rwop) || IMG_isCUR(rwop) || IMG_isGIF(rwop) ||
			IMG_isICO(rwop) || IMG_isLBM(rwop) || IMG_isPCX(rwop) || 
			IMG_isPNM(rwop) || IMG_isSVG(rwop) || IMG_isWEBP(rwop) ||
			IMG_isXCF(rwop) || IMG_isXPM(rwop) || IMG_isXV(rwop))
		{
			return true;
		}
		return false;
	}
private:
	SDL_Surface* textureSurface;
	SDLSystem* sdlSystem;
	std::string path;
};



class DrawTexture
{
public:

private:

};



class InputSystem
{
public:
	enum class Key : Uint32
	{
		A = SDLK_a,
		B = SDLK_b,
		C = SDLK_c,
		D = SDLK_d,
		E = SDLK_e,
		F = SDLK_f,
		G = SDLK_g,
		H = SDLK_h,
		I = SDLK_i,
		J = SDLK_j,
		K = SDLK_k,
		L = SDLK_l,
		M = SDLK_m,
		N = SDLK_n,
		O = SDLK_o,
		P = SDLK_p,
		Q = SDLK_q,
		R = SDLK_r,
		S = SDLK_s,
		T = SDLK_t,
		U = SDLK_u,
		V = SDLK_v,
		W = SDLK_w,
		X = SDLK_x,
		Y = SDLK_y,
		Z = SDLK_z,
	};
	void update()
	{
		while (SDL_PollEvent(&inputEvent) != 0)
		{
			if (inputEvent.type == SDL_QUIT)
			{
				pushFlag = true;
			}
		}
	}
	bool push()
	{
		return pushFlag;
	}
private:
	SDL_Event inputEvent;
	bool pushFlag = false;
};



int main(int /*argc*/, char** /*argv*/)
{
	ECS::EntitySystemManager::get().initialize();

	std::unique_ptr<ECS::EntityManager> deviceManager = std::make_unique<ECS::EntityManager>();
	ECS::EntitySystemManager::get().regist("device", deviceManager);

	ECS::EntityManager* manager = &ECS::EntitySystemManager::get().getEntityManager("device");
	ECS::Entity* deviceEntity = &manager->addEntity();
	deviceEntity->addComponent<ECS::Color<>>(0x00,0xFF,0xFF);
	deviceEntity->addComponent<ECS::Screen>("window", Vector2(1080, 720));
	deviceEntity->addComponent<SDLSystem>();
	deviceEntity->addComponent<LoadTexture>("sample.png");

	InputSystem input;
	while(1)
	{
		ECS::EntitySystemManager::get().update();
		input.update();
		if (input.push())
		{
			break;
		}
	}
	return 0;
}



