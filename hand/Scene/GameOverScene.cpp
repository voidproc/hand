﻿#include "GameOverScene.h"
#include "SceneSize.h"
#include "Theme.h"
#include "Hiscore.h"
#include "AudioPlay.h"

namespace hand
{
	GameOverScene::GameOverScene(const InitData& init)
		:
		IScene{ init },
		time_{ StartImmediately::Yes, GlobalClock::Get() }
	{
		// 無音のシーンなので一応 BGM を止めておく
		StopAllBgm(0s);

		SaveHiscore(getData());
	}

	void GameOverScene::update()
	{
		if (time_ > 1s)
		{
			if (getData().input.decide().down())
			{
				changeScene(U"TitleScene", 0s);

				AudioAsset(U"Change").playOneShot();
			}
		}
	}

	void GameOverScene::draw() const
	{
		SceneRect.draw(Theme::Black);

		if (time_ > 1s)
		{
			FontAsset(U"StageTitle")(U"ＧＡＭＥ ＯＶＥＲ").drawAt(SceneCenter + Vec2{ 0, -12 }, Theme::White);

			const auto textScore = U"{:08d}"_fmt(getData().score);
			FontAsset(U"H68")(textScore).drawAt(SceneCenter + Vec2{ 0, 10 + 1 }, Theme::Darker);
			FontAsset(U"H68")(textScore).drawAt(SceneCenter + Vec2{ 0, 10 + 0 }, Theme::White);
		}
	}
}
