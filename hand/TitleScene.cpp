﻿#include "TitleScene.h"
#include "SceneSize.h"
#include "Theme.h"

namespace hand
{
	namespace
	{
		constexpr int ItemCount = 2;
	}

	TitleScene::TitleScene(const InitData& init)
		:
		IScene{ init },
		time_{ StartImmediately::Yes },
		timeEnter_{ StartImmediately::No },
		timeCursor_{ StartImmediately::No },
		cursor_{ 0 }
	{
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

				AudioAsset(U"Select").play();
			}

			if (getData().input.rightDown())
			{
				cursor_ = (cursor_ + 1) % ItemCount;
				timeCursor_.restart();

				AudioAsset(U"Select").play();
			}

			if (getData().input.decide().down())
			{
				if (cursor_ == 0)
				{
					timeEnter_.start();

					AudioAsset(U"Decide").play();
				}
				else if (cursor_ == 1)
				{
					AudioAsset(U"Select").play();

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
				}
				return U"";
				};

			const auto text = FontAsset(U"H88")(labelFunc(cursor_));
			const double vibX = 2.0 * (1.0 - Clamp(timeCursor_.sF() / 0.18, 0.0, 1.0)) * Periodic::Sine1_1(0.06s);
			const auto region = text.regionAt(SceneCenter.movedBy(0, 30));
			text.draw(region.pos.movedBy(vibX, 0), ColorF{Theme::Black, alpha});

			const double arrowAlpha = timeEnter_.isStarted() ? 0.0 : Periodic::Square0_1(0.75s);
			TextureAsset(U"ArrowLeft").drawAt(region.leftCenter().movedBy(-16, 0), ColorF{ Theme::Black, arrowAlpha });
			TextureAsset(U"ArrowLeft").mirrored().drawAt(region.rightCenter().movedBy(16, 0), ColorF{ Theme::Black, arrowAlpha });
		}

		// フェードイン
		{
			const double t = 1.0 - Clamp(time_.sF() / 0.5, 0.0, 1.0);
			SceneRect.draw(ColorF{ Theme::Black, static_cast<int>(4.0 * t) / 4.0 });
		}

	}
}
