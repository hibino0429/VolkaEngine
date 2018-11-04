#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")
#include <SDL.h>
#include <iostream>

#include <SDL2_image/SDL_image.h>
#include "src/ECS/ECS.hpp"
#include "src/ComponentSystem/StandardComponents.hpp"
#include "src/ComponentSystem/EngineComponents.hpp"
#include "src/ComponentSystem/Renderer.hpp"

#pragma comment(lib, "SDL2_image.lib")
//#pragma comment(lib, "SDL2_mixer.lib")
//#pragma comment(lib, "SDL2_ttf.lib")




#define SDL_PrintError(name)                                    \
  do {                                                          \
    std::cerr << #name << ": " << SDL_GetError() << std::endl;  \
  } while (0)







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
		: textureSurface(nullptr), window(nullptr)
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
		SDL_Surface* surface = &window->GetSurface();
		SDL_BlitSurface(textureSurface, nullptr, surface, nullptr);
		
	}
	[[noreturn]] void initialize() noexcept override
	{
		if (!entity->hasComponent<ECS::WindowSystem>())
		{
			entity->addComponent<ECS::WindowSystem>();
		}
		window = &entity->getComponent<ECS::WindowSystem>();
		
		int imgFlags = IMG_INIT_PNG;
		int initted = IMG_Init(imgFlags);
		//initialize png loading
		if ((initted & imgFlags) != imgFlags)
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
	[[nodiscard]] SDL_Surface* LoadSurface(const std::string& filePath) const noexcept
	{
		//final optimized image
		SDL_Surface* optimizedSurface = nullptr;
		//load image path
		SDL_Surface* loadSurface = IMG_Load(filePath.c_str());
		if (loadSurface == nullptr)
		{
			SDL_PrintError(loadSurface);
		}
		else
		{
			SDL_Surface* surface = &window->GetSurface();
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
	ECS::WindowSystem* window;
	std::string path;
};






class CreateTexture final : public ECS::ComponentSystem
{
public:
	[[noreturn]] void initialize() noexcept override
	{
		//set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf_s("Warning: linear texture filtering not enabled");
		}
		else
		{
			//initialize png loading
			int imgFlags = IMG_INIT_PNG;
			int initted = IMG_Init(imgFlags);
			if ((initted & imgFlags) != imgFlags)
			{
				printf_s("SDL_image could not intialized");
			}
		}
		renderer = &entity->getComponent<ECS::RendererSystem>();
		texList = &entity->getComponent<ECS::AssetTexture>();
	}
	[[noreturn]] void update() noexcept override {}
	[[noreturn]] void draw2D() noexcept override {}
	[[noreturn]] void draw3D() noexcept override {}
public:
	[[nodiscard]] SDL_Texture& process(const std::string& filePath) const noexcept
	{
		SDL_Texture* newTexture = nullptr;
		SDL_Surface* loadSurface = IMG_Load(filePath.c_str());
		if (loadSurface == nullptr)
		{
			SDL_PrintError(loadSurface);
		}
		else
		{
			newTexture = SDL_CreateTextureFromSurface(&renderer->GetRenderer(), loadSurface);
			if (newTexture == nullptr)
			{
				SDL_PrintError(newTexture);
			}
			//old load surface free
			SDL_FreeSurface(loadSurface);
		}
		return *newTexture;
	}
	[[noreturn]] void regist(const std::string& name,const std::string& filePath) noexcept
	{
		SDL_Texture* texture = &process(filePath);
		texList->regist(name, texture);
	}
private:
	ECS::RendererSystem* renderer;
	ECS::AssetTexture* texList;
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
	ECS::EntitySystemManager::get().regist("engine", deviceManager);

	ECS::EntityManager* manager = &ECS::EntitySystemManager::get().getEntityManager("engine");
	ECS::Entity* deviceEntity = &manager->addEntity();
	deviceEntity->addComponent<ECS::Color>((Uint8)0xFF, (Uint8)0xFF, (Uint8)0xFF, (Uint8)0xFF);
	deviceEntity->addComponent<ECS::Screen>("window", Vector2(1080, 720));
	deviceEntity->addComponent<ECS::WindowSystem>();
	deviceEntity->addComponent<ECS::RendererSystem>();
	deviceEntity->addComponent<ECS::AssetTexture>();
	deviceEntity->addComponent<CreateTexture>().regist("samp","Resource/test.bmp");
	deviceEntity->getComponent<CreateTexture>().regist("te", "Resource/sample.png");
	deviceEntity->addGroup(ENTITY_GROUP::Engine);

	//gameManager
	std::unique_ptr<ECS::EntityManager> gameManager = std::make_unique<ECS::EntityManager>();
	ECS::EntitySystemManager::get().regist("game", gameManager);
	ECS::EntityManager* game = &ECS::EntitySystemManager::get().getEntityManager("game");
	
	//rect
	ECS::Entity* entity = &game->addEntity();
	entity->addComponent<ECS::Color>().setColor(Vector4Type<Uint8>(0xFF, 0x00, 0x00, 0xFF));
	entity->addComponent<ECS::GeometryRenderer>(Vector4(50,100,100,50),ECS::GeometryRenderer::Mode::Fill);
	entity->addComponent<ECS::DrawTexture>("te").setRect(Vector4(0,0,128,128),Vector4(0,0,640,480));

	//renderer (最後でないとバグる)
	ECS::Entity* render = &game->addEntity();
	render->addComponent<ECS::Renderer>();
	render->addGroup(ENTITY_GROUP::Engine);

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



