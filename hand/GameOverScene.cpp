#include "GameOverScene.h"
#include "SceneSize.h"
#include "Theme.h"

namespace hand
{
	GameOverScene::GameOverScene(const InitData& init)
		:
		IScene{ init },
		time_{ StartImmediately::Yes }
	{
	}

	void GameOverScene::update()
	{
		if (time_ > 1s)
		{
			if ((KeyEnter | KeySpace).down())
			{
				changeScene(U"TitleScene", 0s);
			}
		}
	}

	void GameOverScene::draw() const
	{
		SceneRect.draw(Theme::Black);

		FontAsset(U"StageTitle")(U"ＧＡＭＥ ＯＶＥＲ").drawAt(SceneCenter, Theme::White);
	}
}
