#include "ConfigScene.h"
#include "SceneSize.h"
#include "Theme.h"

namespace hand
{
	namespace
	{
		constexpr int ItemCount = 3;
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
		if (getData().input.up().down())
		{
			cursor_ = (cursor_ - 1 + ItemCount) % ItemCount;
		}

		if (getData().input.down().down())
		{
			cursor_ = (cursor_ + 1) % ItemCount;
		}


		Config& config = getData().config;

		switch (cursor_)
		{
		case 0:
			// ウィンドウサイズの変更
			if (getData().input.left().down())
			{
				config.windowScale = Clamp(config.windowScale - 1, 1, 8);
				Window::Resize((SceneSize * config.windowScale));
			}

			if (getData().input.right().down())
			{
				config.windowScale = Clamp(config.windowScale + 1, 1, 8);
				Window::Resize((SceneSize * config.windowScale));
			}
			break;

		case 1:
			// エフェクト使用
			if ((getData().input.left() | getData().input.right()).down())
			{
				config.useEffect = not config.useEffect;
			}
			break;

		case 2:
			// 戻る
			if (getData().input.decide().down())
			{
				// TODO: 設定保存

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

		Vec2 itemPos{ PaddingL, 24 };

		const auto drawItem = [&](int index, StringView label, StringView value) {
			if (index == cursor_)
			{
				RectF{ 0, itemPos.y - 2, SizeF{ SceneWidth, LineHeight } }.draw(ColorF{ Theme::Lighter, 0.5 + 0.5 * Periodic::Jump0_1(0.5s) });
			}

			FontAsset(U"H88")(label).draw(itemPos, Theme::Black);
			FontAsset(U"H88")(value)
				.draw(Arg::topRight = Vec2{ SceneWidth - PaddingR, itemPos.y }, Theme::Black);

			itemPos.y += LineHeight;
			};


		Config& config = getData().config;

		drawItem(0, U"WINDOW SIZE", Format(U"x", config.windowScale));
		drawItem(1, U"USE SCREEN FX", config.useEffect ? U"ON" : U"OFF");
		drawItem(2, U"BACK TO TITLE", U"");
	}
}
