//---------------------------------------------------------
//!@file Renderer.hpp
//!@brief 標準的なRendererのComponent群を用意します
//!@author Volka
//!@date 2018/11/3
//!@note 
//---------------------------------------------------------
#pragma once
#include "../ECS/ECS.hpp"
#include "../ComponentSystem/EngineComponents.hpp"

namespace ECS
{
	//!@class Renderer
	//!@brief 描画を行えるようにします
	class Renderer : public ComponentSystem
	{
	public:
		void initialize() noexcept override
		{
			auto& e = ECS::EntitySystemManager::get().getEntityManager("engine").getEntitiesByGroup(ENTITY_GROUP::Engine).at(0);
			renderer = &e->getComponent<RendererSystem>();
			color = &e->getComponent<Color>();
		}
		void update() noexcept override
		{
			SDL_SetRenderDrawColor(&renderer->GetRenderer(), color->value.xValue, color->value.yValue, color->value.zValue, color->value.wValue);
			SDL_RenderClear(&renderer->GetRenderer());
		}
		void draw2D() noexcept override
		{
			SDL_RenderPresent(&renderer->GetRenderer());
		}
		void draw3D()noexcept override {}
	private:
		ECS::RendererSystem* renderer;
		ECS::Color* color;
	};

	//!@class GeometryRenderer
	//!@brief ジオメトリを描画します
	class GeometryRenderer : public ComponentSystem
	{
	public:
		enum class Mode
		{
			Fill,		//塗りつぶし
			Wireframe,	//ワイヤーフレーム
			Line,		//線
			Point,		//点
		};
	public:
		explicit GeometryRenderer() noexcept
		{
			mode = Mode::Fill;
			rect = { 0,0,0,0 };
		}
		explicit GeometryRenderer(const Vector4& drawRect, const Mode& renderMode) noexcept
		{
			rect.x = static_cast<int>(drawRect.xValue);
			rect.y = static_cast<int>(drawRect.yValue);
			rect.w = static_cast<int>(drawRect.zValue);
			rect.h = static_cast<int>(drawRect.wValue);
			mode = renderMode;
		}
		[[noreturn]] void initialize() noexcept override
		{
			ECS::EntityManager* manager = &ECS::EntitySystemManager::get().getEntityManager("engine");
			auto& engine = manager->getEntitiesByGroup(ENTITY_GROUP::Engine).at(0);
			renderer = &engine->getComponent<RendererSystem>();
			if (!entity->hasComponent<Color>())
			{
				entity->addComponent<Color>().setColor(Vector4Type<Uint8>(0xFF, 0xFF, 0xFF, 0xFF));
			}
			color = &entity->getComponent<Color>();
		}
		[[noreturn]] void update() noexcept override {}
		[[noreturn]] void draw3D() noexcept override {}
		[[noreturn]] void draw2D() noexcept override
		{
			SDL_SetRenderDrawColor(&renderer->GetRenderer(), color->value.xValue, color->value.yValue, color->value.zValue, color->value.wValue);
			switch (mode)
			{
			case Mode::Fill: SDL_RenderFillRect(&renderer->GetRenderer(), &rect); break;
			case Mode::Wireframe: SDL_RenderDrawRect(&renderer->GetRenderer(), &rect); break;
			case Mode::Line: SDL_RenderDrawLine(&renderer->GetRenderer(), rect.x, rect.y, rect.w, rect.h); break;
			case Mode::Point: SDL_RenderDrawPoint(&renderer->GetRenderer(), rect.x, rect.y); break;
			}
		}
	private:
		SDL_Rect rect;
		Mode mode;
		RendererSystem* renderer;
		Color* color;
	};

	//!@class DrawTexture
	//!@brief textureを描画します
	class DrawTexture : public ComponentSystem
	{
	public:
		explicit DrawTexture(const AssetTexture::KeyTypeData& key) noexcept
		{
			accessKey = key;
		}
		[[noreturn]] void initialize() noexcept override
		{
			auto& engine = EntitySystemManager::get().getEntityManager("engine").getEntitiesByGroup(ENTITY_GROUP::Engine).at(0);
			renderer = &engine->getComponent<RendererSystem>();
			texList = &engine->getComponent<AssetTexture>();
		}
		[[noreturn]] void update() noexcept override {}
		[[noreturn]] void draw2D() noexcept override
		{
			SDL_RenderCopy(&renderer->GetRenderer(), texList->getTexture(accessKey), &srcRectData, &drawRectData);
		}
		[[noreturn]] void draw3D() noexcept override {}
		//!@brief drawRectの設定
		[[noreturn]] void setDrawRect(const Vector4& rect) noexcept
		{
			drawRectData.x = static_cast<int>(rect.xValue);
			drawRectData.y = static_cast<int>(rect.yValue);
			drawRectData.w = static_cast<int>(rect.zValue);
			drawRectData.h = static_cast<int>(rect.wValue);
		}
		//!@brief srcRectの設定
		[[noreturn]] void setSrcRect(const Vector4& rect) noexcept
		{
			srcRectData.x = static_cast<int>(rect.xValue);
			srcRectData.y = static_cast<int>(rect.yValue);
			srcRectData.w = static_cast<int>(rect.zValue);
			srcRectData.h = static_cast<int>(rect.wValue);
		}
		//!@brief [drawRect]と[srcRect]を設定
		[[noreturn]] void setRect(const Vector4& drawRect, const Vector4& srcRect) noexcept
		{
			setDrawRect(drawRect);
			setSrcRect(srcRect);
		}
	private:
		RendererSystem* renderer;
		AssetTexture* texList;
		AssetTexture::KeyTypeData accessKey;
		SDL_Rect drawRectData;
		SDL_Rect srcRectData;
	};

	//!@class SpriteRenderer
	//!@brief スプライトを描画します
	class SpriteRenderer : public ComponentSystem
	{
	public:
		[[noreturn]] void initialize() noexcept override
		{

		}
		[[noreturn]] void update() noexcept override
		{

		}
		[[noreturn]] void draw2D() noexcept override
		{

		}
		[[noreturn]] void draw3D() noexcept override
		{

		}
	private:
	};

}