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
		timeVibHeader_{ StartImmediately::No },
		timeSlide_{ StartImmediately::Yes },
		difficulty_{ Difficulty::Normal }
	{
		timeVibHeader_.set(999s);
	}

	void RankingScene::update()
	{
		if (getData().input.leftDown())
		{
			difficulty_ = DifficultyPrev(difficulty_);
			timeVibHeader_.restart();
			timeSlide_.restart();

			AudioAsset(U"Select").playOneShot();
		}
		else if (getData().input.rightDown())
		{
			difficulty_ = DifficultyNext(difficulty_);
			timeVibHeader_.restart();
			timeSlide_.restart();

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

		// 選択難易度によってヘッダの色を変える
		{
			constexpr double GlowWidth = 48.0;
			constexpr double GlowShear = 4.0;
			const double shiftX = GlowWidth * (difficulty_ == Difficulty::Easy ? -1.0 : (difficulty_ == Difficulty::Hard ? 1.0 : 0.0));
			const double alpha = Clamp(timeSlide_.sF() / 0.1, 0.0, 1.0);
			const ColorF color0{ Theme::Darker, 0 * alpha };
			const ColorF color1{ Theme::Darker, 0.75 * alpha };
			headerRect.scaled(0.25, 1.0).shearedX(GlowShear).movedBy(shiftX - GlowWidth / 2 + GlowShear, 0).draw(color0, color1, color1, color0);
			headerRect.scaled(0.25, 1.0).shearedX(GlowShear).movedBy(shiftX + GlowWidth / 2 - GlowShear, 0).draw(color1, color0, color0, color1);
		}

		FontAsset(U"H88")(U"RANKING - {}"_fmt(DifficultyToNameString(difficulty_)))
			.drawAt(headerRect.center().movedBy(GetTextVibrationOnSelectionChange(timeVibHeader_.sF())), Theme::White);

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

			// スコアなどはモード切替時にスライドしてくる
			{
				const double t = 1.0 - EaseOutExpo(Clamp((timeSlide_.sF() - iEntry * 0.03) / 0.3, 0.0, 1.0));
				const Transformer2D translator{ Mat3x2::Translate(t * SceneWidth, 0) };

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
}
