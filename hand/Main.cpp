#include <Siv3D.hpp> // OpenSiv3D v0.6.12
#include "SceneSize.h"
#include "TitleScene.h"
#include "ConfigScene.h"
#include "MainScene.h"
#include "GameOverScene.h"
#include "SandboxScene.h"
#include "Theme.h"

void InitSivSystem()
{
	Scene::SetBackground(hand::Theme::Black);

	Window::SetTitle(U"The HAND of Salvation - 救いの手 | v1.0.0");

	// ESCキーで終了しない
	System::SetTerminationTriggers(UserAction::CloseButtonClicked);
}

void LoadAssets()
{
	FontAsset::Register(U"Title", 20, U"font/JF-Dot-Kappa20.ttf", FontStyle::BoldBitmap);
	FontAsset::Register(U"Sub", 8, U"font/misaki_mincho.ttf", FontStyle::Bitmap);
	FontAsset::Register(U"Goh", 12, U"font/JF-Dot-ShinonomeMin12.ttf", FontStyle::Bitmap);
	FontAsset::Register(U"StageTitle", 12, U"font/JF-Dot-ShinonomeMin12.ttf", FontStyle::BoldBitmap);
	FontAsset::Register(U"Score", 8, U"font/Score.ttf", FontStyle::Bitmap);
	FontAsset::Register(U"Config", 10, U"font/JF-Dot-MPlusH10.ttf", FontStyle::Bitmap);

	TextureAsset::Register(U"Girl", U"texture/girl.png");
	TextureAsset::Register(U"Airplane", U"texture/airplane.png");
	TextureAsset::Register(U"Hand", U"texture/hand.png");
	TextureAsset::Register(U"BgMountain", U"texture/bg_mountain.png");
	TextureAsset::Register(U"BgMountain2", U"texture/bg_mountain2.png");
	TextureAsset::Register(U"BgTree", U"texture/bg_tree.png");
	TextureAsset::Register(U"KarmaGaugeFrame", U"texture/karma_gauge_frame.png");
	TextureAsset::Register(U"KarmaGauge", U"texture/karma_gauge.png");
	TextureAsset::Register(U"Bird", U"texture/bird.png");
	TextureAsset::Register(U"Money", U"texture/money.png");
	TextureAsset::Register(U"Bullet", U"texture/bullet.png");
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
	app.add<TitleScene>(U"TitleScene");
	app.add<ConfigScene>(U"ConfigScene");
	app.add<MainScene>(U"MainScene");
	app.add<GameOverScene>(U"GameOverScene");
	//app.add<SandboxScene>(U"SandboxScene");
	app.setFadeColor(Theme::Black);

	//app.init(U"TitleScene", 0s);
	app.init(U"ConfigScene", 0s);
	//app.init(U"MainScene", 0s);
	//app.init(U"SandboxScene", 0s);

	// Config
	auto data = app.get().get();
	data->config.windowScale = DefaultWindowScale;
	data->config.useEffect = true;

	// シェーダ（スキャンラインエフェクト用）
	const PixelShader ps = HLSL{ U"shader/scanline.hlsl", U"PS_Texture" };
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
