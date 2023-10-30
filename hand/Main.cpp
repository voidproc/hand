#include <Siv3D.hpp> // OpenSiv3D v0.6.12
#include "SceneSize.h"
#include "SplashScene.h"
#include "TitleScene.h"
#include "ConfigScene.h"
#include "MainScene.h"
#include "GameOverScene.h"
#include "SandboxScene.h"
#include "Theme.h"
#include "Res.h"

void InitSivSystem()
{
	Scene::SetBackground(hand::Theme::Black);

	Window::SetTitle(U"The HAND of Salvation - 救いの手 | v1.0.0");

	// ESCキーで終了しない
	System::SetTerminationTriggers(UserAction::CloseButtonClicked);
}

void LoadAssets()
{
	TextureAsset::Register(U"Powered", RES(U"texture/powered.png"));
	TextureAsset::Load(U"PlayerIndicator");

	FontAsset::Register(U"Title", 20, RES(U"font/JF-Dot-Kappa20.ttf"), FontStyle::BoldBitmap);
	FontAsset::Register(U"Sub", 8, RES(U"font/misaki_mincho.ttf"), FontStyle::Bitmap);
	FontAsset::Register(U"Goh", 12, RES(U"font/JF-Dot-ShinonomeMin12.ttf"), FontStyle::Bitmap);
	FontAsset::Register(U"StageTitle", 12, RES(U"font/JF-Dot-ShinonomeMin12.ttf"), FontStyle::BoldBitmap);
	FontAsset::Register(U"H68", 8, RES(U"font/hand6x8_2.ttf"), FontStyle::Bitmap);
	FontAsset::Register(U"H68Thin", 8, RES(U"font/hand6x8.ttf"), FontStyle::Bitmap);
	FontAsset::Register(U"H88", 8, RES(U"font/hand8x8.ttf"), FontStyle::Bitmap);

	TextureAsset::Register(U"Girl", RES(U"texture/girl.png"));
	TextureAsset::Register(U"Airplane", RES(U"texture/airplane.png"));
	TextureAsset::Register(U"Hand", RES(U"texture/hand.png"));
	TextureAsset::Register(U"BgMountain", RES(U"texture/bg_mountain.png"));
	TextureAsset::Register(U"BgMountain2", RES(U"texture/bg_mountain2.png"));
	TextureAsset::Register(U"BgTree", RES(U"texture/bg_tree.png"));
	TextureAsset::Register(U"KarmaGaugeFrame", RES(U"texture/karma_gauge_frame.png"));
	TextureAsset::Register(U"KarmaGauge", RES(U"texture/karma_gauge.png"));
	TextureAsset::Register(U"Bird", RES(U"texture/bird.png"));
	TextureAsset::Register(U"BirdB", RES(U"texture/bird_b.png"));
	TextureAsset::Register(U"Money", RES(U"texture/money.png"));
	TextureAsset::Register(U"Bullet", RES(U"texture/bullet.png"));
	TextureAsset::Register(U"ArrowLeft", RES(U"texture/arrow_left.png"));
	TextureAsset::Register(U"PlayerIndicator", RES(U"texture/player_indicator.png"));
	TextureAsset::Register(U"JellyFish", RES(U"texture/jellyfish.png"));
}

void Main()
{
	using namespace hand;

	InitSivSystem();

	LoadAssets();

	// 低解像度のシーン
	Scene::Resize(SceneSize * RenderTextureScale);
	Scene::SetTextureFilter(TextureFilter::Nearest);
	const ScopedRenderStates2D renderState{ SamplerState::ClampNearest };
	RenderTexture renderTexture{ SceneSize };

	// ウィンドウサイズ
	constexpr double DefaultWindowScale = 3;
	Window::Resize((SceneSize * DefaultWindowScale).asPoint());

	// アプリケーション
	App app;
	app.add<SplashScene>(U"SplashScene");
	app.add<TitleScene>(U"TitleScene");
	app.add<ConfigScene>(U"ConfigScene");
	app.add<MainScene>(U"MainScene");
	app.add<GameOverScene>(U"GameOverScene");
	//app.add<SandboxScene>(U"SandboxScene");
	app.setFadeColor(Theme::Black);

	//app.init(U"SplashScene", 0s);
	//app.init(U"TitleScene", 0s);
	//app.init(U"ConfigScene", 0s);
	app.init(U"MainScene", 0s);
	//app.init(U"SandboxScene", 0s);

	// Config
	auto data = app.get().get();
	data->config.windowScale = DefaultWindowScale;
	data->config.useEffect = true;

	// シェーダ（スキャンラインエフェクト用）
	const PixelShader ps = HLSL{ RES(U"shader/scanline.hlsl"), U"PS_Texture" };
	if (not ps)
	{
		Logger << U"PixelShaderの作成に失敗";
	}

	while (System::Update())
	{
		{
			const ScopedRenderTarget2D renderTarget{ renderTexture };

			// シーンを更新
			if (not app.update())
			{
				break;
			}
		}

		{
			const Transformer2D scaler{ Mat3x2::Scale(data->config.windowScale) };

			const bool useEffect = data->config.useEffect && data->config.windowScale >= 2;
			const ScopedCustomShader2D shader = useEffect ? ScopedCustomShader2D{ ps } : ScopedCustomShader2D{};

			renderTexture.draw();
		}

		Circle{ Scene::CenterF(), Scene::Width() * Math::Sqrt2 / 2 * 1.2 }.draw(ColorF{ 0, 0 }, ColorF{ 0, 0.20 });
	}
}
