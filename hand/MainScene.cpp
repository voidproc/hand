﻿#include "MainScene.h"
#include "SceneSize.h"
#include "Theme.h"

namespace hand
{
	namespace
	{
		constexpr double ScoreRateMin = 1.0;
		constexpr double ScoreRateMax = 8.0;
		constexpr double ScoreRateDecrOnTimer = 0.1;
		constexpr auto ScoreRateDecrSpeed = 0.22s;
	}

	MainScene::MainScene(const InitData& init)
		:
		IScene{ init },
		effect_{},
		player_{ hands_ },
		hands_{},
		enemies_{},
		items_{},
		time_{ StartImmediately::Yes },
		timerSpawnEnemy_{ 5s, StartImmediately::Yes },
		timePlayerDead_{ StartImmediately::No },
		timerShake_{ 0.4s, StartImmediately::No },
		scoreRate_{ ScoreRateMin },
		timeIncrScoreRate_{ StartImmediately::No },
		timerDecrScoreRate_{ ScoreRateDecrSpeed, StartImmediately::Yes }
	{
		getData().currentStage += 1;
	}

	void MainScene::update()
	{
		player_.update();

		for (auto& hand : hands_)
		{
			hand->update();
		}

		for (int iEnemy = 0; iEnemy < enemies_.size(); ++iEnemy)
		{
			enemies_[iEnemy]->update();
		}

		for (auto& item : items_)
		{
			item->update();
		}

		// スコアレート
		updateScoreRate_();

		// 衝突判定
		checkCollision_();

		// 期限切れの Hand を破棄
		hands_.remove_if([](const auto& hand) { return not hand->isAlive(); });

		// 期限切れの Enemy を破棄
		enemies_.remove_if([](const auto& enemy) { return not enemy->isAlive(); });

		// 期限切れの Item を破棄
		items_.remove_if([](const auto& item) { return not item->isAlive(); });

		// プレイヤーが倒されてから数秒後にシーン移行
		if (timePlayerDead_ >= 4s)
		{
			changeScene(U"GameOverScene", 0s);
			return;
		}

		// [DEBUG] 敵をランダムに生成
		if (timerSpawnEnemy_.reachedZero())
		{
			timerSpawnEnemy_.restart(SecondsF{ Random(0.5, 2.0) });

			enemies_.emplace_back(MakeEnemy<Bird2, EnemyType::Bird2>(effect_, enemies_, Vec2{SceneWidth + 16, Random(32.0, SceneHeight - 32.0)}));
		}
	}

	void MainScene::draw() const
	{
		// BG Color
		Scene::Rect().draw(Theme::White);


		const Transformer2D shaker{ Mat3x2::Translate(RandomVec2(timerShake_.isRunning() ? 4.0 * EaseInCubic(timerShake_.progress1_0()) : 0.0))};

		// BG Texture
		drawBG_();

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
		drawStageTitle_();

		// ステータス
		drawKarma_();
		drawScore_();

		// ゲームオーバーへ移行直前のフェードアウト
		if (timePlayerDead_ > 2.0s)
		{
			constexpr double FadeTime = 2.0;
			const double t = Clamp((timePlayerDead_.sF() - 2.0) / FadeTime, 0.0, 1.0);
			SceneRect.draw(ColorF{ Theme::Black, t });
		}
	}

	void MainScene::updateScoreRate_()
	{
		if (not hands_.isEmpty())
		{
			if (not timeIncrScoreRate_.isRunning()) timeIncrScoreRate_.restart();

			// 加算する倍率
			const double rateIncr = 16.0 * EaseInSine(Clamp(timeIncrScoreRate_.sF(), 0.0, 4.0) / 4.0) * Scene::DeltaTime();
			scoreRate_ = Clamp(scoreRate_ + rateIncr, ScoreRateMin, ScoreRateMax);
		}
		else
		{
			timeIncrScoreRate_.reset();

			if (timerDecrScoreRate_.reachedZero())
			{
				timerDecrScoreRate_.restart();
				scoreRate_ = Clamp(scoreRate_ - ScoreRateDecrOnTimer, ScoreRateMin, ScoreRateMax);
			}
		}
	}

	void MainScene::checkCollision_()
	{
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
						// 敵を撃破したのでスコアを加算する
						getData().score += EnemyScore(enemy->type()) * scoreRate_;

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

		// Player の衝突判定は、Player が生きてる時だけ
		if (player_.isAlive())
		{
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

					// アイテムを取得したのでスコアを加算する
					getData().score += ItemScore(item->type()) * scoreRate_;

					item->kill();
				}
			}

			// 衝突判定 - Player vs Enemy
			for (auto& enemy : enemies_)
			{
				if (not player_.isInvincible() && player_.collision().intersects(enemy->collision()))
				{
					shake_();

					player_.damage(20.0);

					if (not player_.isAlive())
					{
						timePlayerDead_.start();
						break;
					}
				}
			}
		}
	}

	void MainScene::drawBG_() const
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

	void MainScene::drawStageTitle_() const
	{
		if (time_ < 3s)
		{
			const double x = SceneCenter.x + 200 * Clamp(EaseInCubic(1.0 - time_.sF() / 0.6), 0.0, 1.0);
			const double xOut = (time_ > 2.4s) ? 200 * EaseInCubic((time_.sF() - 2.4) / 0.6) : 0.0;

			const auto stageTextFunc = [](int stage) {
				switch (stage)
				{
				case 1: return std::make_pair<String, String>(U"ＳＴＡＧＥ １", U"- Pleasant Sunny Day -");
				case 2: return std::make_pair<String, String>(U"ＳＴＡＧＥ ２", U"- Tearful Rain Passage -");
				case 3: return std::make_pair<String, String>(U"ＳＴＡＧＥ ３", U"- Back to Nostalgic Day -");
				}
				return std::make_pair<String, String>(U"", U"");
				};
			const auto stageText = stageTextFunc(getData().currentStage);

			FontAsset(U"StageTitle")(stageText.first).drawAt(x - xOut + 1, SceneCenter.y + 1 - 8, Theme::Lighter);
			FontAsset(U"StageTitle")(stageText.first).drawAt(x - xOut, SceneCenter.y - 8, Theme::Black);
			FontAsset(U"Sub")(stageText.second).drawAt(x - xOut, SceneCenter.y + 20 - 8, Theme::Black);
		};
	}

	void MainScene::drawKarma_() const
	{
		const bool isKarmaLow = (player_.karma() < Player::KarmaEmptyThreshold);
		const Color karmaLabelColor = isKarmaLow ? Theme::Darker.lerp(Theme::Black, Periodic::Square0_1(0.3s)) : Theme::Black;

		// カルマ
		FontAsset(U"Goh")(U"業").draw(2, 0, Theme::Darker);
		FontAsset(U"Goh")(U"業").draw(1, 0, karmaLabelColor);
		FontAsset(U"Sub")(U"KARMA").draw(14, 7, karmaLabelColor);

		// カルマゲージ枠
		const Color dangerColor = (player_.karma() <= Player::KarmaDanger) ? Palette::White.lerp(Theme::Lighter, Periodic::Square0_1(0.3s)) : Palette::White;
		const auto frameRegion = TextureAsset(U"KarmaGaugeFrame").draw(15, 1, dangerColor);

		// カルマゲージ
		int gaugeWidth = 42 * player_.karma() / 100.0;
		if (player_.karma() > 1e-3 && gaugeWidth == 0)
		{
			gaugeWidth = 1;
		}

		if (isKarmaLow)
		{
			FontAsset(U"Sub")(U"EMPTY").drawAt(frameRegion.center().movedBy(9, 0), ColorF{ Theme::Black, Periodic::Square0_1(0.3s) });
		}

		TextureAsset(U"KarmaGauge")(0, 0, gaugeWidth, 10).draw(16, 1, dangerColor);
	}

	void MainScene::drawScore_() const
	{
		// スコア
		const String scoreText = U"{:08d}"_fmt(getData().score);
		FontAsset(U"Score")(scoreText).drawAt(SceneWidth / 2 + 14 + 1, 6 + 1, Theme::Lighter);
		FontAsset(U"Score")(scoreText).drawAt(SceneWidth / 2 + 14 + 0, 6 + 0, Theme::Black);

		// 倍率
		FontAsset(U"Sub")(U"x{:.1f}"_fmt(scoreRate_)).drawAt(SceneWidth - 10, 6 + 0, Theme::Black);
	}

	void MainScene::shake_()
	{
		timerShake_.restart();
	}
}
