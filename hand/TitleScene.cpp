#include "TitleScene.h"
#include "SceneSize.h"
#include "Theme.h"

namespace hand
{
	TitleScene::TitleScene(const InitData& init)
		: IScene{ init }, time_{ StartImmediately::Yes }, timeEnter_{ StartImmediately::No }
	{
	}

	void TitleScene::update()
	{
		if (time_ > 1s)
		{
			if (KeyEnter.down())
			{
				timeEnter_.start();
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
			const int alpha = timeEnter_.isStarted() ? Periodic::Square0_1(0.2s) * 255 : 255;
			const int alpha2 = (not timeEnter_.isStarted()) ? Periodic::Square0_1(0.75s) * 255 : alpha;

			const auto text = FontAsset(U"Sub")(U"Press Enter Key");
			const auto region = text.regionAt(SceneCenter.movedBy(0, 32));
			text.draw(region.pos, Theme::Black.withAlpha(alpha).withAlpha(alpha2));
		}
		
	}
}
