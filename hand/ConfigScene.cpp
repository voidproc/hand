#include "ConfigScene.h"
#include "SceneSize.h"
#include "Theme.h"
#include "AudioVolume.h"

namespace hand
{
	namespace
	{
		constexpr int ItemCount = 6;
	}

	ConfigScene::ConfigScene(const InitData& init)
		:
		IScene{ init },
		time_{ StartImmediately::Yes },
		cursor_{ 0 }
	{
	}

	void ConfigScene::update()
	{
		if (getData().input.upDown())
		{
			cursor_ = (cursor_ - 1 + ItemCount) % ItemCount;

			AudioAsset(U"Select").play();
		}

		if (getData().input.downDown())
		{
			cursor_ = (cursor_ + 1) % ItemCount;

			AudioAsset(U"Select").play();
		}


		Config& config = getData().config;

		switch (cursor_)
		{
		case 0:
			// ウィンドウサイズの変更
			if (getData().input.leftDown())
			{
				config.windowScale = Clamp(config.windowScale - 1, 1, 8);
				Window::Resize((SceneSize * config.windowScale));

				AudioAsset(U"Select").play();
			}

			if (getData().input.rightDown())
			{
				config.windowScale = Clamp(config.windowScale + 1, 1, 8);
				Window::Resize((SceneSize * config.windowScale));

				AudioAsset(U"Select").play();
			}
			break;

		case 1:
			// エフェクト使用
			if (getData().input.leftDown() || getData().input.rightDown())
			{
				config.useEffect = not config.useEffect;

				AudioAsset(U"Select").play();
			}
			break;

		case 2:
			// SE ボリューム変更
			if (getData().input.leftDown())
			{
				config.seVolume = Clamp(config.seVolume - 10, 0, 100);
				ApplyAudioVolume(config.seVolume, config.bgmVolume);

				AudioAsset(U"Select").play();
			}

			if (getData().input.rightDown())
			{
				config.seVolume = Clamp(config.seVolume + 10, 0, 100);
				ApplyAudioVolume(config.seVolume, config.bgmVolume);

				AudioAsset(U"Select").play();
			}
			break;

		case 3:
			// BGM ボリューム変更
			if (getData().input.leftDown())
			{
				config.bgmVolume = Clamp(config.bgmVolume - 10, 0, 100);
				ApplyAudioVolume(config.seVolume, config.bgmVolume);

				AudioAsset(U"Select").play();
			}

			if (getData().input.rightDown())
			{
				config.bgmVolume = Clamp(config.bgmVolume + 10, 0, 100);
				ApplyAudioVolume(config.seVolume, config.bgmVolume);

				AudioAsset(U"Select").play();
			}
			break;

		case 4:
			// コントローラ ID
			if (getData().input.leftDown())
			{
				config.controllerId = Clamp(config.controllerId - 1, 0, 3);

				AudioAsset(U"Select").play();
			}

			if (getData().input.rightDown())
			{
				config.controllerId = Clamp(config.controllerId + 1, 0, 3);

				AudioAsset(U"Select").play();
			}
			break;

		case 5:
			// 戻る
			if (getData().input.decide().down())
			{
				AudioAsset(U"Select").play();

				// 設定保存
				config.save(Config::ConfigFilePath);

				changeScene(U"TitleScene", 0s);
			}
			break;
		}
	}

	void ConfigScene::draw() const
	{
		SceneRect.draw(Theme::White);

		// ヘッダ
		Rect{ 0, 0, SceneWidth, 16 }.draw(Theme::Black);
		FontAsset(U"StageTitle")(U"設定").draw(4, 1, Theme::White);
		FontAsset(U"Sub")(U"Configuration").draw(34, 8, Theme::White);

		// 設定項目...

		constexpr int PaddingL = 8;
		constexpr int PaddingR = 8;
		constexpr int LineHeight = 12;

		Vec2 itemPos{ PaddingL, 28 };

		const auto drawItem = [&](int index, StringView label, StringView value, const Color& color = Theme::Black) {
			if (index == cursor_)
			{
				RectF{ 0, itemPos.y - 2, SizeF{ SceneWidth, LineHeight } }.draw(ColorF{ Theme::Lighter, 0.5 + 0.5 * Periodic::Jump0_1(0.5s) });
			}

			FontAsset(U"H88")(label).draw(itemPos, Theme::Black);
			FontAsset(U"H88")(value)
				.draw(Arg::topRight = Vec2{ SceneWidth - PaddingR, itemPos.y }, color);

			itemPos.y += LineHeight + 2;
			};


		Config& config = getData().config;

		drawItem(0, U"WINDOW SIZE", Format(U"x", config.windowScale));
		drawItem(1, U"USE SCREEN FX", config.useEffect ? U"ON" : U"OFF");
		drawItem(2, U"SE VOLUME", Format(config.seVolume));
		drawItem(3, U"BGM VOLUME", Format(config.bgmVolume));
		drawItem(4, U"CONTROLLER ID", Format(config.controllerId), XInput(config.controllerId).isConnected() ? Theme::Black : Theme::Darker);
		drawItem(5, U"BACK TO TITLE", U"");
	}
}
