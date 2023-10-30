﻿#include "SplashScene.h"
#include "SceneSize.h"
#include "Theme.h"

namespace hand
{
	SplashScene::SplashScene(const InitData& init)
		:
		IScene{ init },
		time_{ StartImmediately::Yes, GlobalClock::Get() }
	{
	}

	void SplashScene::update()
	{
		if (time_ > 3.2s + 1.2s + 1.3s || (time_ > 1.0s && getData().input.decide().down()))
		{
			changeScene(U"TitleScene", 0s);
		}
	}

	void SplashScene::draw() const
	{
		SceneRect.draw(Theme::White);

		if (time_ < 0.09s)
		{
			for (int i : step(Random(3)))
			{
				const double y = Random(SceneHeight);
				Line{ 0, y, SceneWidth, y }.draw(Theme::Black);
			}
		}

		if (time_ > 0.75s)
		{
			TextureAsset(U"Powered")(0, 0, 80, 8).draw(160 / 2 - 80 / 2, 144 / 2 - 80 / 2);
		}

		if (time_ > 1.20s)
		{
			const double t = Clamp((time_.sF() - 1.2) / 1.2, 0.0, 1.0);
			TextureAsset(U"Powered")(0, 8, 80, 56).draw(160 / 2 - 80 / 2, 144 / 2 - 80 / 2 + 8, AlphaF(static_cast<int>(4.0 * t) / 4.0));
		}

		if (time_ > 2.4s + 0.8s)
		{
			TextureAsset(U"Powered")(0, 8 + 56, 80, 16).draw(160 / 2 - 80 / 2, 144 / 2 - 80 / 2 + 8 + 56);
		}

		if (time_ > 3.2s + 1.2s)
		{
			const double t = Clamp((time_.sF() - 3.2 - 1.2) / 1.0, 0.0, 1.0);
			SceneRect.draw(ColorF{ Theme::White, static_cast<int>(4.0 * t) / 4.0 });
		}
	}
}