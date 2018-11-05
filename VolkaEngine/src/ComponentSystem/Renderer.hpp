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
	class Renderer final : public ComponentSystem
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
	class GeometryRenderer final : public ComponentSystem
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

	//!@class RotationTexture
	//!@brief textureを回転させます
	class RotationTexture final : public ComponentData
	{
	public:
		enum class FlipMode
		{
			None,		//フリップしない
			Horizontal, //水平にフリップ
			Vertical,	//垂直にフリップ
		};
	public:
		explicit RotationTexture() noexcept
		{
			angle = 0.0f;
			pivot = Vector2(0, 0);
			flipMode = FlipMode::None;
		}
		explicit RotationTexture(const float rotateAngle, const Vector2& pivotPoint, const FlipMode flip = FlipMode::None) noexcept
		{
			angle = rotateAngle;
			pivot = pivotPoint;
			flipMode = flip;
		}
	public:
		//!@brief 角度の取得
		[[nodiscard]] const float getAngle() const noexcept
		{
			return angle;
		}
		//!@brief ピボット(回転の中心点)の取得
		[[nodiscard]] const Vector2& getPivot() const noexcept
		{
			return pivot;
		}
		//!@brief フリップモードの取得
		[[nodiscard]] const FlipMode& getFlipMode() const noexcept
		{
			return flipMode;
		}
		//!@brief 角度の設定
		[[noreturn]] void setAngle(const float rotateAngle) noexcept
		{
			angle = rotateAngle;
		}
		//!@brief ピボット(回転の中心点)の設定
		[[noreturn]] void setPivot(const Vector2& pivotPoint) noexcept
		{
			pivot = pivotPoint;
		}
		[[noreturn]] void setFlipMode(const FlipMode& flip) noexcept
		{
			flipMode = flip;
		}
	private:
		float angle;
		Vector2 pivot;
		FlipMode flipMode;
	};

	//!@class DrawTexture
	//!@brief textureを描画します
	class DrawTexture final : public ComponentSystem
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
			rotation = nullptr;
		}
		[[noreturn]] void update() noexcept override {}
		[[noreturn]] void draw2D() noexcept override
		{
			SDL_Rect drawRect = Convert(drawRectData);
			SDL_Rect srcRect = Convert(srcRectData);
			if (entity->hasComponent<RotationTexture>())
			{
				if (rotation == nullptr)
				{
					rotation = &entity->getComponent<RotationTexture>();
				}
				double angle = static_cast<double>(rotation->getAngle());
				SDL_Point pivot = { 0,0 };
				pivot.x = static_cast<int>(rotation->getPivot().xValue);
				pivot.y = static_cast<int>(rotation->getPivot().yValue);
				SDL_RendererFlip flip = SDL_FLIP_NONE;
				switch (rotation->getFlipMode())
				{
				case RotationTexture::FlipMode::None: flip = SDL_FLIP_NONE; break;
				case RotationTexture::FlipMode::Horizontal: flip = SDL_FLIP_HORIZONTAL; break;
				case RotationTexture::FlipMode::Vertical: flip = SDL_FLIP_VERTICAL; break;
				}
				SDL_RenderCopyEx(&renderer->GetRenderer(), texList->getTexture(accessKey), &srcRect, &drawRect,angle,&pivot,flip);
			}
			else
			{
				SDL_RenderCopy(&renderer->GetRenderer(), texList->getTexture(accessKey), &srcRect, &drawRect);
			}
		}
		[[noreturn]] void draw3D() noexcept override {}
		//!@brief drawRectの設定
		[[noreturn]] void setDrawRect(const Vector4& rect) noexcept
		{
			drawRectData = rect;
		}
		//!@brief srcRectの設定
		[[noreturn]] void setSrcRect(const Vector4& rect) noexcept
		{
			srcRectData = rect;
		}
		//!@brief [drawRect]と[srcRect]を設定
		[[noreturn]] void setRect(const Vector4& drawRect, const Vector4& srcRect) noexcept
		{
			setDrawRect(drawRect);
			setSrcRect(srcRect);
		}
		//!@brief アクセスした[key]の[value]を取得します
		[[nodiscard]] const AssetTexture::ValueTypeData& getValue() const noexcept
		{
			return texList->getTexture(accessKey);
		}
		//!@brief [drawRect]を取得します
		[[nodiscard]] const Vector4& getDrawRect() const noexcept
		{
			return drawRectData;
		}
		//!@brief [srcRect]を取得します
		[[nodiscard]] const Vector4& getSrcRect() const noexcept
		{
			return srcRectData;
		}
	private:
		//!@brief コンバート
		const SDL_Rect Convert(const Vector4& vector4) const noexcept
		{
			SDL_Rect rect;
			rect.x = static_cast<int>(vector4.xValue);
			rect.y = static_cast<int>(vector4.yValue);
			rect.w = static_cast<int>(vector4.zValue);
			rect.h = static_cast<int>(vector4.wValue);
			return rect;
		}
	private:
		RendererSystem* renderer;
		AssetTexture* texList;
		AssetTexture::KeyTypeData accessKey;
		RotationTexture* rotation;
		Vector4 drawRectData;
		Vector4 srcRectData;
	};

	//!@class SpriteSheetRenderer
	//!@brief スプライトを描画します
	class SpriteSheetRenderer final : public ComponentSystem
	{
	public:
		explicit SpriteSheetRenderer() noexcept
		{
			keyFrameData = 0;
			clipNumData = 0;
		}
		explicit SpriteSheetRenderer(const int keyFrame, const int clipNum) noexcept
		{
			keyFrameData = keyFrame;
			clipNumData = clipNum;
		}
		[[noreturn]] void initialize() noexcept override
		{
			animFrame = 0;
			nowClip = 0;
			texture = &entity->getComponent<DrawTexture>();
			clipData = texture->getSrcRect();
		}
		[[noreturn]] void update() noexcept override
		{
			++animFrame;
			nowClip = animFrame / keyFrameData % clipNumData;
			Vector4 animRect = { clipData.xValue + clipData.wValue * nowClip,clipData.yValue,clipData.zValue,clipData.wValue };
			texture->setSrcRect(animRect);
		}
		[[noreturn]] void draw2D() noexcept override {}
		[[noreturn]] void draw3D() noexcept override {}
		[[noreturn]] void setKeyFrame(const int keyFrame) noexcept
		{
			keyFrameData = keyFrame;
		}
		[[noreturn]] void setClipNum(const int clipNum) noexcept
		{
			clipNumData = clipNum;
		}
	private:
		DrawTexture* texture;
		Vector4 clipData;
		int keyFrameData;
		int animFrame;
		int clipNumData;
		int nowClip;
	};

	//!@class BlendMode
	class BlendMode final : public ComponentSystem
	{
	public:
		enum class Mode
		{
			None,		//ブレンドなし
			AlphaBlend, //アルファブレンド
			Add,		//加法混和
			Mod,		//変調
		};
	public:
		explicit BlendMode() noexcept
		{
			blendMode = SDL_BLENDMODE_NONE;
			alphaBlendValue = 255;
		}
		explicit BlendMode(const Mode& mode) noexcept
		{
			setBlendMode(mode);
			alphaBlendValue = 255;
		}
		explicit BlendMode(const Mode& mode, const int alpha) noexcept
		{
			setBlendMode(mode);
			setAlpha(alpha);
		}
		[[noreturn]] void initialize() noexcept override
		{
			texture = &entity->getComponent<DrawTexture>();
			SDL_SetTextureBlendMode(texture->getValue(), blendMode);
		}
		[[nodiscard]] void update() noexcept override
		{
			if (blendMode == SDL_BLENDMODE_NONE) { return; }
			if (alphaBlendValue < 0) { alphaBlendValue = 0; }
			if (alphaBlendValue > 255) { alphaBlendValue = 255; }
			SDL_SetTextureAlphaMod(texture->getValue(), static_cast<Uint8>(alphaBlendValue));
		}
		[[noreturn]] void draw2D() noexcept override {}
		[[noreturn]] void draw3D() noexcept override {}
		//!@brief ブレンドモードの設定
		[[noreturn]] void setBlendMode(const Mode& mode) noexcept
		{
			switch (mode)
			{
			case Mode::None: blendMode = SDL_BLENDMODE_NONE; break;
			case Mode::AlphaBlend: blendMode = SDL_BLENDMODE_BLEND; break;
			case Mode::Add: blendMode = SDL_BLENDMODE_ADD; break;
			case Mode::Mod: blendMode = SDL_BLENDMODE_MOD; break;
			}
		}
		//!@brief Alpha値の設定
		[[noreturn]] void setAlpha(const int alpha) noexcept
		{
			alphaBlendValue = alpha;
		}
	private:
		SDL_BlendMode blendMode;
		int alphaBlendValue;
		DrawTexture* texture;
	};

	

	//!@class ControlViewport
	//!@brief ビューポートを制御する
	class ControlViewport final : public ComponentSystem
	{
	public:
		explicit ControlViewport() noexcept
		{
			viewportRect = { 0, 0, 0, 0 };
		}
		[[noreturn]] void initialize() noexcept override
		{
			auto& engine = EntitySystemManager::get().getEntityManager("engine").getEntitiesByGroup(ENTITY_GROUP::Engine).at(0);
			renderer = &engine->getComponent<RendererSystem>();
		}
		[[noreturn]] void update() noexcept override {}
		[[noreturn]] void draw2D() noexcept override
		{
			SDL_RenderSetViewport(&renderer->GetRenderer(), &viewportRect);
		}
		[[noreturn]] void draw3D() noexcept override {}
		[[noreturn]] void setViewport(const Vector4& viewport) noexcept
		{
			viewportRect.x = static_cast<int>(viewport.xValue);
			viewportRect.y = static_cast<int>(viewport.yValue);
			viewportRect.w = static_cast<int>(viewport.zValue);
			viewportRect.h = static_cast<int>(viewport.wValue);
		}
	private:
		SDL_Rect viewportRect;
		RendererSystem* renderer;
	};
}