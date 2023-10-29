#include "GameOverScene.h"
#include "SceneSize.h"
#include "Theme.h"

namespace hand
{
	GameOverScene::GameOverScene(const InitData& init)
		:
		IScene{ init },
		time_{ StartImmediately::Yes, GlobalClock::Get() }
	{

		getData().currentStage = 0;
	}

	void GameOverScene::update()
	{
		if (time_ > 1s)
		{
			if (getData().input.decide().down())
			{
				changeScene(U"TitleScene", 0s);
			}
		}
	}

	void GameOverScene::draw() const
	{
		SceneRect.draw(Theme::Black);

		if (time_ > 1s)
		{
			FontAsset(U"StageTitle")(U"ＧＡＭＥ ＯＶＥＲ").drawAt(SceneCenter, Theme::White);
		}
	}
}
