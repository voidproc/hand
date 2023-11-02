#include "SplashScene.h"
#include "SceneSize.h"
#include "Theme.h"

namespace hand
{
	namespace
	{
		void PreloadAssets()
		{
			for (auto [name, info] : FontAsset::Enumerate())
			{
				FontAsset::LoadAsync(name, U"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz,./;:[](){}@-=+*<>?!#$%&_");
			}

			for (auto [name, info] : TextureAsset::Enumerate())
			{
				TextureAsset::LoadAsync(name);
			}
		}

		bool IsCompletedPreloadingAssets()
		{
			for (auto [name, info] : FontAsset::Enumerate())
			{
				if (not FontAsset::IsReady(name)) return false;
			}

			for (auto [name, info] : TextureAsset::Enumerate())
			{
				if (not TextureAsset::IsReady(name)) return false;
			}

			return true;
		}
	}

	SplashScene::SplashScene(const InitData& init)
		:
		IScene{ init },
		time_{ StartImmediately::Yes, GlobalClock::Get() },
		timerCheckAsset_{ 1s, StartImmediately::Yes }
	{
		PreloadAssets();
	}

	void SplashScene::update()
	{
		if (timerCheckAsset_.reachedZero())
		{
			if (not IsCompletedPreloadingAssets())
			{
				timerCheckAsset_.restart();
			}
			else
			{
				timerCheckAsset_.reset();
			}
		}

		if (timerCheckAsset_.isRunning()) return;

		if (time_ > 2.8s + 1.0s + 0.5s + 0.7s || (time_ > 1.0s && getData().input.decide().down()))
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

		if (time_ > 0.60s)
		{
			TextureAsset(U"Powered")(0, 0, 80, 8).draw(160 / 2 - 80 / 2, 144 / 2 - 80 / 2);
		}

		if (time_ > 1.00s)
		{
			const double t = Clamp((time_.sF() - 1.0) / 1.0, 0.0, 1.0);
			TextureAsset(U"Powered")(0, 8, 80, 56).draw(160 / 2 - 80 / 2, 144 / 2 - 80 / 2 + 8, AlphaF(static_cast<int>(4.0 * t) / 4.0));
		}

		if (time_ > 2.0s + 0.8s)
		{
			TextureAsset(U"Powered")(0, 8 + 56, 80, 16).draw(160 / 2 - 80 / 2, 144 / 2 - 80 / 2 + 8 + 56);
		}

		if (time_ > 2.8s + 1.0s)
		{
			const double t = Clamp((time_.sF() - 2.8 - 1.0) / 0.5, 0.0, 1.0);
			SceneRect.draw(ColorF{ Theme::Black, static_cast<int>(4.0 * t) / 4.0 });
		}

		if (timerCheckAsset_.isRunning())
		{
			FontAsset(U"Sub")(U"Loading assets", String(1u + static_cast<size_t>(2.9 * Periodic::Sawtooth0_1(0.5s)), U'.'))
				.draw(Arg::bottomLeft = Vec2{ 1, SceneHeight }, Theme::Darker);
		}
	}
}
