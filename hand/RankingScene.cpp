#include "RankingScene.h"
#include "SceneSize.h"
#include "Theme.h"
#include "ScoreText.h"
#include "TextVibration.h"

namespace hand
{
	namespace
	{
		StringView GetOrdinal(int num)
		{
			switch (num)
			{
			case 1: return U"ST"_sv;
			case 2: return U"ND"_sv;
			case 3: return U"RD"_sv;
			}

			return U"TH"_sv;
		}
	}

	RankingScene::RankingScene(const InitData& init)
		:
		IScene{ init },
		time_{ StartImmediately::Yes },
		timeCursor_{ StartImmediately::No },
		difficulty_{ Difficulty::Normal }
	{
		timeCursor_.set(999s);
	}

	void RankingScene::update()
	{
		if (getData().input.leftDown())
		{
			difficulty_ = DifficultyPrev(difficulty_);
			timeCursor_.restart();

			AudioAsset(U"Select").playOneShot();
		}
		else if (getData().input.rightDown())
		{
			difficulty_ = DifficultyNext(difficulty_);
			timeCursor_.restart();

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

		// ヘッダ
		const auto headerRect = Rect{ 0, 0, SceneWidth, 16 }.draw(Theme::Black);
		FontAsset(U"H88")(U"RANKING - {}"_fmt(DifficultyToNameString(difficulty_)))
			.drawAt(headerRect.center().movedBy(GetTextVibrationOnSelectionChange(timeCursor_.sF())), Theme::White);

		// 矢印
		const double arrowAlpha = Periodic::Square0_1(0.75s);
		TextureAsset(U"ArrowLeft").drawAt(headerRect.leftCenter().movedBy(8, 0), ColorF{ Theme::White, arrowAlpha });
		TextureAsset(U"ArrowLeft").mirrored().drawAt(headerRect.rightCenter().movedBy(-8, 0), ColorF{ Theme::White, arrowAlpha });

		for (int iEntry : step(ResultList::EntryCount))
		{
			const auto& result = getData().result.get(difficulty_, iEntry);

			constexpr int LineHeight = 14;

			const Vec2 lineCenter{ SceneCenter.x, 30 + iEntry * LineHeight };
			const RectF lineRect{ Arg::center = lineCenter, SizeF{ SceneWidth - 16, 14 } };

			// 背景の影
			const auto shadeRect = lineRect.movedBy(0, 8).setSize(lineRect.w, 4);
			shadeRect.rounded(2.0).draw(ColorF{ Theme::Lighter, 0.5 });

			// スコア
			DrawScoreTextAt(lineCenter.movedBy(-8, 0), result.score);

			// 順位
			const auto numRect = FontAsset(U"H68")(iEntry + 1)
				.drawAt(lineRect.leftCenter().movedBy(6, 0), Theme::Black);

			FontAsset(U"H68Thin")(GetOrdinal(iEntry + 1))
				.draw(numRect.tr().movedBy(0, 1), Theme::Darker);

			// エリア
			FontAsset(U"H68Thin")(AreaToString(result.area))
				.drawAt(lineRect.rightCenter().movedBy(-20, 1), Theme::Black);
		}
	}
}
