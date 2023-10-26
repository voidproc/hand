#include "MainScene.h"
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
		obj_{},
		time_{ StartImmediately::Yes, GlobalClock::Get() },
		timerSpawnEnemy_{ 5s, StartImmediately::Yes, GlobalClock::Get() },
		timePlayerDead_{ StartImmediately::No, GlobalClock::Get() },
		timerShake_{ 0.4s, StartImmediately::No, GlobalClock::Get() },
		scoreRateRaw_{ ScoreRateMin },
		timeIncrScoreRate_{ StartImmediately::No, GlobalClock::Get() },
		timerDecrScoreRate_{ ScoreRateDecrSpeed, StartImmediately::Yes, GlobalClock::Get() },
		eventList_{ obj_ }
	{
		getData().currentStage += 1;

		eventList_.load(U"event/stage1.csv");
	}

	void MainScene::update()
	{
		// ポーズのためのカスタムクロック
		auto clock = GlobalClock::Get();

		// ポーズ切替
		if (KeyEnter.down())
		{
			if (clock->isPaused())
			{
				clock->start();
			}
			else
			{
				clock->pause();
			}
		}

		// ポーズ中はシーンを更新しない
		if (clock->isPaused()) return;


		// 各オブジェクトの更新…

		obj_.player.update();

		for (auto& hand : obj_.hands)
		{
			hand->update();
		}

		for (int iEnemy = 0; iEnemy < obj_.enemies.size(); ++iEnemy)
		{
			obj_.enemies[iEnemy]->update();
		}

		for (auto& item : obj_.items)
		{
			item->update();
		}

		// スコアレート
		updateScoreRate_();

		// 衝突判定
		checkCollision_();

		// 期限切れの Hand を破棄
		obj_.hands.remove_if([](const auto& hand) { return not hand->isAlive(); });

		// 期限切れの Enemy を破棄
		obj_.enemies.remove_if([](const auto& enemy) { return not enemy->isAlive(); });

		// 期限切れの Item を破棄
		obj_.items.remove_if([](const auto& item) { return not item->isAlive(); });

		// プレイヤーが倒されてから数秒後にシーン移行
		if (timePlayerDead_ >= 4s)
		{
			changeScene(U"GameOverScene", 0s);
			return;
		}

		// ステージイベント
		while (eventList_.update(time_.sF())) {}
	}

	void MainScene::draw() const
	{
		// BG Color
		Scene::Rect().draw(Theme::White);

		const bool paused = GlobalClock::Get()->isPaused();

		const auto shakeAmount = (timerShake_.isRunning() && not paused) ? 4.0 * EaseInCubic(timerShake_.progress1_0()) : 0.0;
		const Transformer2D shaker{ Mat3x2::Translate(RandomVec2(shakeAmount))};

		// BG Texture
		drawBG_();

		// Player
		obj_.player.draw();

		// Hands
		for (const auto& hand : obj_.hands)
		{
			hand->draw();
		}

		// エフェクト
		obj_.effect.update();

		// 敵や弾
		for (const auto& enemy : obj_.enemies)
		{
			enemy->draw();
		}

		// アイテム
		for (const auto& item : obj_.items)
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

		// ポーズ
		if (paused)
		{
			SceneRect.draw(ColorF{ Theme::Black, 0.8 });
			FontAsset(U"Sub")(U"PAUSE").drawAt(SceneCenter, Theme::White);
		}
	}

	void MainScene::updateScoreRate_()
	{
		if (not obj_.hands.isEmpty())
		{
			if (not timeIncrScoreRate_.isRunning()) timeIncrScoreRate_.restart();

			// 加算する倍率
			const double rateIncr = 16.0 * EaseInSine(Clamp(timeIncrScoreRate_.sF(), 0.0, 4.0) / 4.0) * Scene::DeltaTime();
			scoreRateRaw_ = Clamp(scoreRateRaw_ + rateIncr, ScoreRateMin, ScoreRateMax);
		}
		else
		{
			timeIncrScoreRate_.reset();

			if (timerDecrScoreRate_.reachedZero())
			{
				timerDecrScoreRate_.restart();
				scoreRateRaw_ = Clamp(scoreRateRaw_ - ScoreRateDecrOnTimer, ScoreRateMin, ScoreRateMax);
			}
		}
	}

	void MainScene::checkCollision_()
	{
		// 衝突判定 - Hand vs Enemy
		for (auto& hand : obj_.hands)
		{
			for (auto& enemy : obj_.enemies)
			{
				if (enemy->isAlive() && hand->collision().intersects(enemy->collision()))
				{
					enemy->damage(100);

					if (not enemy->isAlive())
					{
						// 敵を撃破したのでスコアを加算する
						addScore_(EnemyScore(enemy->type()) * scoreRate_());

						// 敵の撃破後にお金が散らばる
						if (IsEnemy(enemy->type()))
						{
							for (int iMoney : step(4))
							{
								obj_.items.emplace_back(MakeItem<ItemMoney, ItemType::Money>(obj_.effect, enemy->pos()));
							}
						}
					}
				}
			}
		}

		// Player の衝突判定は、Player が生きてる時だけ
		if (obj_.player.isAlive())
		{
			// 衝突判定 - Player vs Item
			for (auto& item : obj_.items)
			{
				if (obj_.player.collision().intersects(item->collision()) ||
					obj_.player.collisionAirplane().intersects(item->collision()))
				{
					switch (item->type())
					{
					case ItemType::Money:
						obj_.player.addKarma(Player::KarmaRecoveryOnGetMoney);
						break;
					}

					// アイテムを取得したのでスコアを加算する
					addScore_(ItemScore(item->type()) * scoreRate_());

					item->kill();
				}
			}

			// 衝突判定 - Player vs Enemy
			for (auto& enemy : obj_.enemies)
			{
				if (not obj_.player.isInvincible() && obj_.player.collision().intersects(enemy->collision()))
				{
					shake_();

					obj_.player.damage(20.0);

					// このダメージでプレイヤーがしんでしまった
					if (not obj_.player.isAlive())
					{
						timePlayerDead_.start();
						break;
					}

					// ダメージを受けるとスコアレートが半分になる
					scoreRateRaw_ = Clamp(scoreRateRaw_ / 2, ScoreRateMin, ScoreRateMax);
				}
			}
		}
	}

	void MainScene::drawBG_() const
	{
		const ScopedRenderStates2D sampler{ SamplerState::RepeatNearest };

		TextureAsset(U"BgMountain2")
			.mapped(640, 64)
			.draw(Arg::bottomLeft = Vec2{ -(static_cast<int>(time_.sF() * 12.0) % 320), SceneHeight }, AlphaF(0.5));

		TextureAsset(U"BgMountain")
			.mapped(640, 64)
			.draw(Arg::bottomLeft = Vec2{ -(static_cast<int>(time_.sF() * 20.0) % 320), SceneHeight });

		TextureAsset(U"BgTree")
			.mapped(400, 32)
			.mirrored(true)
			.draw(Arg::bottomLeft = Vec2{ -(static_cast<int>(time_.sF() * 110.0) % 200), SceneHeight + 2 }, AlphaF(0.5));

		TextureAsset(U"BgTree")
			.mapped(400, 32)
			.draw(Arg::bottomLeft = Vec2{ -(static_cast<int>(time_.sF() * 140.0) % 200), SceneHeight + 4 }, AlphaF(1));
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
				case 1: return std::make_pair<String, String>(U"ＳＴＡＧＥ １", U"- SUNNY DAY -");
				case 2: return std::make_pair<String, String>(U"ＳＴＡＧＥ ２", U"- Tearful Rain Passage -");
				case 3: return std::make_pair<String, String>(U"ＳＴＡＧＥ ３", U"- Back to Nostalgic Day -");
				}
				return std::make_pair<String, String>(U"", U"");
				};
			const auto stageText = stageTextFunc(getData().currentStage);

			FontAsset(U"StageTitle")(stageText.first).drawAt(x - xOut + 1, SceneCenter.y + 1 - 8, Theme::Lighter);
			FontAsset(U"StageTitle")(stageText.first).drawAt(x - xOut, SceneCenter.y - 8, Theme::Black);
			FontAsset(U"H68Thin")(stageText.second).drawAt(x - xOut, SceneCenter.y + 20 - 8, Theme::Black);
		};
	}

	void MainScene::drawKarma_() const
	{
		const auto karma = obj_.player.karma();

		const bool isKarmaLow = (karma < Player::KarmaEmptyThreshold);
		const Color karmaLabelColor = isKarmaLow ? Theme::Darker.lerp(Theme::Black, Periodic::Square0_1(0.3s, time_.sF())) : Theme::Black;

		// カルマ
		FontAsset(U"Goh")(U"業").draw(2, 0, Theme::Darker);
		FontAsset(U"Goh")(U"業").draw(1, 0, karmaLabelColor);
		FontAsset(U"Sub")(U"KARMA").draw(14, 7, karmaLabelColor);

		// カルマゲージ枠
		const Color dangerColor = (karma <= Player::KarmaDanger) ? Palette::White.lerp(Theme::Lighter, Periodic::Square0_1(0.3s, time_.sF())) : Palette::White;
		const auto frameRegion = TextureAsset(U"KarmaGaugeFrame").draw(15, 1, dangerColor);

		// カルマゲージ
		int gaugeWidth = 42 * karma / 100.0;
		if (karma > 1e-3 && gaugeWidth == 0)
		{
			gaugeWidth = 1;
		}

		if (isKarmaLow)
		{
			FontAsset(U"Sub")(U"EMPTY").drawAt(frameRegion.center().movedBy(9, 0), ColorF{ Theme::Black, Periodic::Square0_1(0.3s, time_.sF()) });
		}

		TextureAsset(U"KarmaGauge")(0, 0, gaugeWidth, 10).draw(16, 1, dangerColor);
	}

	void MainScene::drawScore_() const
	{
		// スコア
		{
			const String scoreText = U"{:08d}"_fmt(getData().score);
			Vec2 penPos{ 70, 2 };
			bool grayed = true;
			for (auto [index, glyph] : Indexed(FontAsset(U"H68").getGlyphs(scoreText)))
			{
				if (glyph.codePoint != U'0' || index == scoreText.size() - 1) grayed = false;

				glyph.texture.draw(penPos + glyph.getOffset() + Vec2{ 1, 1 }, ColorF{ Theme::Lighter, grayed ? 0.0 : 1.0 });
				glyph.texture.draw(penPos + glyph.getOffset(), grayed ? Theme::Darker : Theme::Black);

				penPos.x += glyph.xAdvance;
			}
		}

		// 倍率
		{
			FontAsset(U"H68Thin")(U"x{:.1f}"_fmt(scoreRateRaw_)).drawAt(SceneWidth - 12, 6 + 0, Theme::Black);
		}
	}

	void MainScene::shake_()
	{
		timerShake_.restart();
	}

	double MainScene::scoreRate_() const
	{
		return static_cast<int>(scoreRateRaw_ * 10) / 10.0;
	}

	void MainScene::addScore_(double score)
	{
		getData().score += static_cast<int>(score / 10) * 10;
	}
}
