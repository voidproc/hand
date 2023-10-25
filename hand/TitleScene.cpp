#include "TitleScene.h"
#include "SceneSize.h"
#include "Theme.h"

namespace hand
{
	namespace
	{
		constexpr int ItemCount = 2;
	}

	TitleScene::TitleScene(const InitData& init)
		: IScene{ init }, time_{ StartImmediately::Yes }, timeEnter_{ StartImmediately::No }, cursor_{ 0 }
	{
		getData().score = 0;
	}

	void TitleScene::update()
	{
		if (time_ > 0.5s)
		{
			if (KeyLeft.down())
			{
				cursor_ = (cursor_ - 1 + ItemCount) % ItemCount;
			}

			if (KeyRight.down())
			{
				cursor_ = (cursor_ + 1) % ItemCount;
			}

			if (KeyEnter.down())
			{
				if (cursor_ == 0)
				{
					timeEnter_.start();
				}
				else if (cursor_ == 1)
				{
					changeScene(U"ConfigScene", 0s);
					return;
				}
			}
		}

		if (timeEnter_ > 1.5s)
		{
			changeScene(U"MainScene", 0s);
		}
	}

	void TitleScene::draw() const
	{
		// 背景全体の色
		Scene::Rect().draw(Theme::White);

		Rect{ 0, 0, SceneWidth, 24 }.draw(Theme::Darker);
		Rect{ Arg::bottomLeft = Point{ 0, SceneHeight }, Size{ SceneWidth, 24 } }.draw(Theme::Darker);

		FontAsset(U"Title")(U"救いの手").drawAt(SceneCenter.movedBy(2, 2 - 16), Theme::Lighter);
		FontAsset(U"Title")(U"救いの手").drawAt(SceneCenter.movedBy(0, 0 - 16), Theme::Black);

		FontAsset(U"Sub")(U"The HAND of Salvation").drawAt(SceneCenter.movedBy(1, 1 + 24 - 16), Theme::Lighter);
		FontAsset(U"Sub")(U"The HAND of Salvation").drawAt(SceneCenter.movedBy(0, 0 + 24 - 16), Theme::Black);

		{
			const double alpha = timeEnter_.isStarted() ? Periodic::Square0_1(0.2s) : 1.0;

			const auto labelFunc = [](int cursor) -> String {
				switch (cursor)
				{
				case 0: return U"Game Start";
				case 1: return U"Configuration";
				}
				return U"";
				};

			const auto text = FontAsset(U"Sub")(labelFunc(cursor_));
			const auto region = text.regionAt(SceneCenter.movedBy(0, 32));
			text.draw(region.pos, ColorF{ Theme::Black, alpha });

			const double arrowAlpha = timeEnter_.isStarted() ? 0.0 : Periodic::Square0_1(0.75s);
			TextureAsset(U"ArrowLeft").drawAt(region.leftCenter().movedBy(-16, 0), ColorF{ Theme::Black, arrowAlpha });
			TextureAsset(U"ArrowLeft").mirrored().drawAt(region.rightCenter().movedBy(16, 0), ColorF{ Theme::Black, arrowAlpha });
		}
		
	}
}
