#include "MainScene.h"
#include "SceneSize.h"
#include "Theme.h"

namespace hand
{
	MainScene::MainScene(const InitData& init)
		:
		IScene{ init },
		effect_{},
		player_{ hands_ },
		hands_{},
		enemies_{},
		items_{},
		time_{ StartImmediately::Yes },
		timerSpawnEnemy_{ 5s, StartImmediately::Yes }
	{
	}

	void MainScene::update()
	{
		player_.update();

		for (auto& hand : hands_)
		{
			hand->update();
		}

		//for (auto& enemy : enemies_)
		for (int iEnemy = 0; iEnemy < enemies_.size(); ++iEnemy)
		{
			enemies_[iEnemy]->update();
		}

		for (auto& item : items_)
		{
			item->update();
		}

		// 衝突判定 - Hand vs Enemy
		for (auto& hand : hands_)
		{
			for (auto& enemy : enemies_)
			{
				if (enemy->isAlive() && hand->collision().intersects(enemy->collision()))
				{
					enemy->damage(100);

					if (not enemy->isAlive())
					{
						// 敵の撃破後にお金が散らばる
						if (IsEnemy(enemy->type()))
						{
							for (int iMoney : step(4))
							{
								items_.emplace_back(MakeItem<ItemMoney, ItemType::Money>(effect_, enemy->pos()));
							}
						}
					}
				}
			}
		}

		// 衝突判定 - Player vs Enemy
		for (auto& enemy : enemies_)
		{
			if (not player_.isInvincible() && player_.collision().intersects(enemy->collision()))
			{
				player_.damage(1);
			}
		}

		// 衝突判定 - Player vs Item
		for (auto& item : items_)
		{
			if (player_.collision().intersects(item->collision()))
			{
				switch (item->type())
				{
				case ItemType::Money:
					player_.addKarma(Player::KarmaRecoveryOnGetMoney);
					break;
				}

				item->kill();
			}
		}

		// 期限切れの Hand を破棄
		hands_.remove_if([](const auto& hand) { return not hand->isAlive(); });

		// 期限切れの Enemy を破棄
		enemies_.remove_if([](const auto& enemy) { return not enemy->isAlive(); });

		// 期限切れの Item を破棄
		items_.remove_if([](const auto& item) { return not item->isAlive(); });


		// [DEBUG] 敵をランダムに生成
		if (timerSpawnEnemy_.reachedZero())
		{
			timerSpawnEnemy_.restart(SecondsF{ Random(0.5, 2.0) });

			enemies_.emplace_back(MakeEnemy<Bird2, EnemyType::Bird2>(effect_, enemies_, Vec2{SceneWidth + 16, Random(32.0, SceneHeight - 32.0)}));
		}
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

		// エフェクト
		effect_.update();

		// 敵や弾
		for (const auto& enemy : enemies_)
		{
			enemy->draw();
		}

		// アイテム
		for (const auto& item : items_)
		{
			item->draw();
		}

		// ステージ表記
		if (time_ < 3s)
		{
			const double x = SceneCenter.x + 200 * Clamp(EaseInCubic(1.0 - time_.sF() / 0.6), 0.0, 1.0);
			const double xOut = (time_ > 2.4s) ? 200 * EaseInCubic((time_.sF() - 2.4) / 0.6) : 0.0;

			const String stageText = U"ＳＴＡＧＥ １";
			FontAsset(U"StageTitle")(stageText).drawAt(x - xOut + 1, SceneCenter.y + 1 - 8, Theme::Lighter);
			FontAsset(U"StageTitle")(stageText).drawAt(x - xOut, SceneCenter.y - 8, Theme::Black);
			FontAsset(U"Sub")(U"- Sunny Day -").drawAt(x - xOut, SceneCenter.y + 20 - 8, Theme::Black);
		}

		// ステータス
		{
			// カルマ
			FontAsset(U"Goh")(U"業").draw(2, 0, Theme::Darker);
			const auto region1 = FontAsset(U"Goh")(U"業").draw(1, 0, Theme::Black);
			const auto region2 = FontAsset(U"Sub")(U"KARMA").draw(14, 7, Theme::Black);

			// カルマゲージ
			TextureAsset(U"KarmaGaugeFrame").draw(15, 1);
			TextureAsset(U"KarmaGauge")(0, 0, 42 * player_.karma() / 100.0, 10).draw(16, 1);

			//FontAsset(U"Goh")(U"快").draw(Arg::topRight = Vec2{ SceneWidth - 1, 0 }, Theme::Darker);
			//FontAsset(U"Goh")(U"快").draw(Arg::topRight = Vec2{ SceneWidth, 0 }, Theme::Black);
		}
	}
}
