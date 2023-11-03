#include <Siv3D.hpp> // OpenSiv3D v0.6.12
#include "SceneSize.h"
#include "SplashScene.h"
#include "TitleScene.h"
#include "ConfigScene.h"
#include "MainScene.h"
#include "GameOverScene.h"
#include "EndingScene.h"
#include "SandboxScene.h"
#include "Theme.h"
#include "Res.h"
#include "DebugMode.h"

void InitSivSystem()
{
	Scene::SetBackground(hand::Theme::Black);

	auto title = U"The HAND of Salvation - 救いの手 | v1.0.0"_sv;
#ifdef DEBUG_MODE
	Window::SetTitle(U"★DEBUG MODE★ - " + title);
#else
	Window::SetTitle(title);
#endif

	// ESCキーで終了しない
	System::SetTerminationTriggers(UserAction::CloseButtonClicked);
}

void LoadAssets()
{
	FontAsset::Register(U"Sub", 8, RES(U"font/misaki_mincho.ttf"), FontStyle::Bitmap);
	FontAsset::Register(U"Goh", 12, RES(U"font/JF-Dot-ShinonomeMin12.ttf"), FontStyle::Bitmap);
	FontAsset::Register(U"StageTitle", 12, RES(U"font/JF-Dot-ShinonomeMin12.ttf"), FontStyle::BoldBitmap);
	FontAsset::Register(U"H68", 8, RES(U"font/hand6x8_2.ttf"), FontStyle::Bitmap);
	FontAsset::Register(U"H68Thin", 8, RES(U"font/hand6x8.ttf"), FontStyle::Bitmap);
	FontAsset::Register(U"H88", 8, RES(U"font/hand8x8.ttf"), FontStyle::Bitmap);

	// Splash
	TextureAsset::Register(U"Powered", RES(U"texture/powered.png"));

	// Title
	TextureAsset::Register(U"TitleBand", RES(U"texture/title_band.png"));
	TextureAsset::Register(U"TitleText", RES(U"texture/title_text.png"));
	TextureAsset::Register(U"ArrowLeft", RES(U"texture/arrow_left.png"));

	// Main
	TextureAsset::Register(U"Girl", RES(U"texture/girl.png"));
	TextureAsset::Register(U"Airplane", RES(U"texture/airplane.png"));
	TextureAsset::Register(U"Hand", RES(U"texture/hand.png"));
	TextureAsset::Register(U"BgMountain", RES(U"texture/bg_mountain.png"));
	TextureAsset::Register(U"BgMountainNight", RES(U"texture/bg_mountain_night.png"));
	TextureAsset::Register(U"BgMountain2", RES(U"texture/bg_mountain2.png"));
	TextureAsset::Register(U"BgTree", RES(U"texture/bg_tree.png"));
	TextureAsset::Register(U"KarmaGaugeFrame", RES(U"texture/karma_gauge_frame.png"));
	TextureAsset::Register(U"KarmaGauge", RES(U"texture/karma_gauge.png"));
	TextureAsset::Register(U"Bird", RES(U"texture/bird.png"));
	TextureAsset::Register(U"BirdB", RES(U"texture/bird_b.png"));
	TextureAsset::Register(U"Money", RES(U"texture/money.png"));
	TextureAsset::Register(U"Bullet", RES(U"texture/bullet2.png"));
	TextureAsset::Register(U"PlayerIndicator", RES(U"texture/player_indicator.png"));
	TextureAsset::Register(U"JellyFish", RES(U"texture/jellyfish.png"));
	TextureAsset::Register(U"NightSky", RES(U"texture/night_sky.png"));
	TextureAsset::Register(U"Stars1", RES(U"texture/stars1.png"));
	TextureAsset::Register(U"Stars2", RES(U"texture/stars2.png"));
	TextureAsset::Register(U"Star1", RES(U"texture/star1.png"));
	TextureAsset::Register(U"Star2", RES(U"texture/star2.png"));
	TextureAsset::Register(U"Bat", RES(U"texture/bat.png"));
	TextureAsset::Register(U"EndBg1", RES(U"texture/end_bg1.png"));

	AudioAsset::Register(U"Select", RES(U"audio/select.wav"));
	AudioAsset::Register(U"Coin", RES(U"audio/coin.ogg"));
	AudioAsset::Register(U"HandShot", RES(U"audio/hand_shot.ogg"));
	AudioAsset::Register(U"Explosion", RES(U"audio/explosion.ogg"));
	AudioAsset::Register(U"Damage", RES(U"audio/damage.ogg"));
	AudioAsset::Register(U"Decide", RES(U"audio/decide.ogg"));
	AudioAsset::Register(U"Bgm1", RES(U"audio/World_without_time.ogg"));
	AudioAsset::Register(U"Bgm2", RES(U"audio/white_darkness.ogg"));
	AudioAsset::Register(U"Bgm3", RES(U"audio/blurred_light.ogg"));
	AudioAsset::Register(U"Ending", RES(U"audio/ending.ogg"));
	AudioAsset::Register(U"Quake", RES(U"audio/quake.ogg"));
	AudioAsset::Register(U"NoiseFade", RES(U"audio/noisefade.ogg"));

	TextureAsset::Load(U"Powered");
	FontAsset::Load(U"Sub");

	GlobalAudio::BusSetVolume(MixBus1, 0.75);
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

	// アプリケーション
	App app;
	app.add<SplashScene>(U"SplashScene");
	app.add<TitleScene>(U"TitleScene");
	app.add<ConfigScene>(U"ConfigScene");
	app.add<MainScene>(U"MainScene");
	app.add<GameOverScene>(U"GameOverScene");
	app.add<EndingScene>(U"EndingScene");
	//app.add<SandboxScene>(U"SandboxScene");
	app.setFadeColor(Theme::Black);

	app.init(U"SplashScene", 0s);
	//app.init(U"TitleScene", 0s);
	//app.init(U"ConfigScene", 0s);
	//app.init(U"MainScene", 0s);
	//app.init(U"GameOverScene", 0s);
	//app.init(U"EndingScene", 0s);
	//app.init(U"SandboxScene", 0s);

	// Config
	auto data = app.get().get();
	auto& config = data->config;
	config.load(Config::ConfigFilePath);

	// ウィンドウサイズ
	Window::Resize(SceneSize * config.windowScale);

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

			data->input.update();
		}

		const bool useEffect = config.useEffect && config.windowScale >= 2;

		{
			const Transformer2D scaler{ Mat3x2::Scale(config.windowScale) };

			const ScopedCustomShader2D shader = useEffect ? ScopedCustomShader2D{ ps } : ScopedCustomShader2D{};

			renderTexture.draw();
		}

		if (useEffect)
		{
			Circle{ Scene::CenterF(), Scene::Width() * Math::Sqrt2 / 2 * 1.2 }.draw(ColorF{ 0, 0 }, ColorF{ 0, 0.20 });
		}
	}
}
