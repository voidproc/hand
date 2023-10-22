#include <Siv3D.hpp> // OpenSiv3D v0.6.12
#include "SceneSize.h"
#include "TitleScene.h"
#include "MainScene.h"

void InitSivSystem()
{
	Scene::SetBackground(Palette::Black);

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

	TextureAsset::Register(U"Girl", U"texture/girl.png");
	TextureAsset::Register(U"Airplane", U"texture/airplane.png");
	TextureAsset::Register(U"Hand", U"texture/hand.png");
	TextureAsset::Register(U"BgMountain", U"texture/bg_mountain.png");
	TextureAsset::Register(U"BgMountain2", U"texture/bg_mountain2.png");
	TextureAsset::Register(U"BgTree", U"texture/bg_tree.png");
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
	app.add<MainScene>(U"MainScene");
	app.setFadeColor(Palette::White);

	//app.init(U"TitleScene", 0s);
	app.init(U"MainScene", 0s);

	// Config
	auto data = app.get().get();
	data->config.windowScale = DefaultWindowScale;

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
			const ScopedCustomShader2D shader = ScopedCustomShader2D{ ps };

			renderTexture.draw();
		}

		Circle{ Scene::CenterF(), Scene::Width() * Math::Sqrt2 / 2 * 1.2 }.draw(ColorF{ 0, 0 }, ColorF{ 0, 0.22 });
	}
}
