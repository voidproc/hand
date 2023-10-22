#include "MainScene.h"
#include "SceneSize.h"
#include "Theme.h"
#include "Hand.h"

namespace hand
{
	MainScene::MainScene(const InitData& init)
		: IScene{ init }, player_{ hands_ }, hands_{}, time_{ StartImmediately::Yes }
	{
	}

	void MainScene::update()
	{
		player_.update();

		for (auto& hand : hands_)
		{
			hand->update();
		}

		// 期限切れの Hand を破棄
		hands_.remove_if([](const auto& hand) { return not hand->isAlive(); });


		PutText(Format(hands_.size()), Vec2{ 8, 8 });
	}

	void MainScene::draw() const
	{
		// BG
		Scene::Rect().draw(Theme::White);

		// BG Texture
		{
			const ScopedRenderStates2D sampler{ SamplerState::RepeatNearest };

			TextureAsset(U"BgMountain2")
				.mapped(640, 64)
				.draw(Arg::bottomLeft = Vec2{ -(static_cast<int>(Scene::Time() * 12.0) % 320), SceneHeight }, AlphaF(0.5));

			TextureAsset(U"BgMountain")
				.mapped(640, 64)
				.draw(Arg::bottomLeft = Vec2{ -(static_cast<int>(Scene::Time() * 20.0) % 320), SceneHeight });

			TextureAsset(U"BgTree")
				.mapped(400, 32)
				.mirrored(true)
				.draw(Arg::bottomLeft = Vec2{ -(static_cast<int>(Scene::Time() * 110.0) % 200), SceneHeight + 2 }, AlphaF(0.5));

			TextureAsset(U"BgTree")
				.mapped(400, 32)
				.draw(Arg::bottomLeft = Vec2{ -(static_cast<int>(Scene::Time() * 140.0) % 200), SceneHeight + 4 }, AlphaF(1));
		}

		// Player
		player_.draw();

		// Hands
		for (const auto& hand : hands_)
		{
			hand->draw();
		}

		// ステージ表記
		if (time_ < 3s)
		{
			const double x = SceneCenter.x + 200 * Clamp(EaseInCubic(1.0 - time_.sF() / 0.6), 0.0, 1.0);
			const double xOut = (time_ > 2.4s) ? 200 * EaseInCubic((time_.sF() - 2.4) / 0.6) : 0.0;

			FontAsset(U"StageTitle")(U"ＳＴＡＧＥ １").drawAt(x - xOut + 1, SceneCenter.y + 1, Theme::Lighter);
			FontAsset(U"StageTitle")(U"ＳＴＡＧＥ １").drawAt(x - xOut, SceneCenter.y, Theme::Black);
		}

		// ステータス
		{
			FontAsset(U"Goh")(U"業").draw(2, 0, Theme::Darker);
			const auto region1 = FontAsset(U"Goh")(U"業").draw(1, 0, Theme::Black);

			const auto region2 = FontAsset(U"Sub")(U"KARMA").draw(14, 7, Theme::Black);
		}
	}
}
