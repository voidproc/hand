#include "RankingScene.h"
#include "SceneSize.h"
#include "Theme.h"
#include "ScoreText.h"

namespace hand
{
	RankingScene::RankingScene(const InitData& init)
		:
		IScene{ init },
		time_{ StartImmediately::Yes },
		difficulty_{ Difficulty::Normal }
	{
	}

	void RankingScene::update()
	{
		if (getData().input.leftDown())
		{
			difficulty_ = DifficultyPrev(difficulty_);

			AudioAsset(U"Select").playOneShot();
		}
		else if (getData().input.rightDown())
		{
			difficulty_ = DifficultyNext(difficulty_);

			AudioAsset(U"Select").playOneShot();
		}

		if (time_ > 1s)
		{
			if (getData().input.decide().down())
			{
				AudioAsset(U"Change").playOneShot();

				changeScene(U"TitleScene", 0s);
			}
		}
	}

	void RankingScene::draw() const
	{
		//BG
		SceneRect.draw(Theme::White);

		FontAsset(U"H88")(DifficultyToNameString(difficulty_)).drawAt(Vec2{ SceneCenter.x, 16 }, Theme::Black);


		for (int iEntry : step(ResultList::EntryCount))
		{
			const auto& result = getData().result.get(difficulty_, iEntry);

			constexpr int LineHeight = 12;

			DrawScoreTextAt(Vec2{ SceneCenter.x, 32 + iEntry * LineHeight }, result.score);
		}
	}
}
