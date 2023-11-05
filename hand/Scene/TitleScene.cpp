#include "TitleScene.h"
#include "SceneSize.h"
#include "Theme.h"
#include "AudioPlay.h"

namespace hand
{
	namespace
	{
		constexpr int ItemCount = 3;
	}

	TitleScene::TitleScene(const InitData& init)
		:
		IScene{ init },
		time_{ StartImmediately::Yes },
		timeEnter_{ StartImmediately::No },
		timeCursor_{ StartImmediately::No },
		cursor_{ 0 }
	{
		// 無音のシーンなので一応 BGM を止めておく
		StopAllBgm(0s);

		getData().score = 0;

		timeCursor_.set(999s);
		timeCursor_.start();
	}

	void TitleScene::update()
	{
		if (time_ > 0.8s)
		{
			if (getData().input.leftDown())
			{
				cursor_ = (cursor_ - 1 + ItemCount) % ItemCount;
				timeCursor_.restart();

				AudioAsset(U"Select").playOneShot();
			}

			if (getData().input.rightDown())
			{
				cursor_ = (cursor_ + 1) % ItemCount;
				timeCursor_.restart();

				AudioAsset(U"Select").playOneShot();
			}

			if (getData().input.decide().down())
			{
				if (cursor_ == 0)
				{
					if (not timeEnter_.isRunning())
					{
						timeEnter_.start();

						AudioAsset(U"Decide").play();
					}
				}
				else if (cursor_ == 1)
				{
					AudioAsset(U"Change").playOneShot();

					changeScene(U"ConfigScene", 0s);
					return;
				}
				else if (cursor_ == 2)
				{
					System::Exit();
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

		TextureAsset(U"TitleBand").draw();
		TextureAsset(U"TitleBand").flipped().draw(Arg::bottomLeft = Vec2{ 0, SceneHeight });
		TextureAsset(U"TitleText").draw(27, 17);

		if (time_ > 0.8s)
		{
			const double alpha = timeEnter_.isStarted() ? Periodic::Square0_1(0.2s) : 1.0;

			const auto labelFunc = [](int cursor) -> String {
				switch (cursor)
				{
				case 0: return U"GAME START";
				case 1: return U"CONFIGURATION";
				case 2: return U"QUIT";
				}
				return U"";
				};

			const auto text = FontAsset(U"H88")(labelFunc(cursor_));
			const double vibX = 2.0 * (1.0 - Clamp(timeCursor_.sF() / 0.18, 0.0, 1.0)) * Periodic::Sine1_1(0.06s);
			const auto region = text.regionAt(SceneCenter.movedBy(0, 32));
			text.draw(region.pos.movedBy(vibX, 1), ColorF{ Theme::Lighter, alpha });
			text.draw(region.pos.movedBy(vibX, 0), ColorF{ Theme::Black, alpha });

			const double arrowAlpha = timeEnter_.isStarted() ? 0.0 : Periodic::Square0_1(0.75s);
			TextureAsset(U"ArrowLeft").drawAt(region.leftCenter().movedBy(-16, 0), ColorF{ Theme::Black, arrowAlpha });
			TextureAsset(U"ArrowLeft").mirrored().drawAt(region.rightCenter().movedBy(16, 0), ColorF{ Theme::Black, arrowAlpha });
		}

		// ハイスコア
		if (getData().config.hiscore > 0)
		{
			const auto textScore = U"HISCORE {:08d}"_fmt(getData().config.hiscore);
			FontAsset(U"H68Thin")(textScore).drawAt(SceneRect.bottomCenter() + Vec2{ 0, -7 + 1 }, Theme::Darker);
			FontAsset(U"H68Thin")(textScore).drawAt(SceneRect.bottomCenter() + Vec2{ 0, -7 + 0 }, Theme::White);
		}

		// フェードイン
		{
			const double t = 1.0 - Clamp(time_.sF() / 0.5, 0.0, 1.0);
			SceneRect.draw(ColorF{ Theme::Black, static_cast<int>(4.0 * t) / 4.0 });
		}

	}
}
