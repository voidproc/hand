#include "MainScene.h"
#include "SceneSize.h"
#include "Theme.h"
#include "Res.h"
#include "SpriteSheet.h"
#include "DebugMode.h"
#include "MsgData.h"

namespace hand
{
	namespace
	{
		constexpr double ScoreRateMin = 1.0;
		constexpr double ScoreRateMax = 8.0;
		constexpr double ScoreRateDecrOnTimer = 0.1;
		constexpr auto ScoreRateDecrTime = 0.22s;
		constexpr auto ScoreRateGaugeIncrOnDestroyEnemy = 0.8s;
		constexpr auto ScoreRateGaugeIncrOnGetMoney = 0.4s;
		constexpr auto ScoreRateGaugeIncrOnCallHand = 0.2s;
		constexpr auto ScoreRateDecrTimeMax = 2.0s;

		constexpr std::array<StringView, 4> StageEventFilePath = {
			U""_sv,
			U"event/stage.csv"_sv,
			U""_sv,
			U""_sv,
		};

		RectF DrawScoreRate(const Vec2& pos, double rate, const Color& color)
		{
			return FontAsset(U"H68Thin")(U"x{:.1f}"_fmt(rate)).drawAt(pos, color);
		}

		struct ScoreRateEffect : IEffect
		{
			ScoreRateEffect(const Vec2& pos, double rate)
				: pos_{ pos }, rate_{ rate }
			{
			}

			bool update(double t) override
			{
				constexpr double Lifetime = 0.65;

				DrawScoreRate(pos_.movedBy(EaseOutCubic(t / Lifetime) * 12.0, 4 + 1), rate_, ColorF{ Theme::White, 0.5 });
				DrawScoreRate(pos_.movedBy(EaseOutCubic(t / Lifetime) * 12.0, 4), rate_, Theme::Black.lerp(Theme::Lighter, Periodic::Jump0_1(0.15s, t)));

				return t < Lifetime;
			}

			Vec2 pos_;
			double rate_;
		};
	}

	// ステージイベント関係
	namespace
	{
		struct GenerateEnemies : IEffect
		{
			GenerateEnemies(std::function<void()> f, double lifetime, double interval)
				:
				f_{ f },
				lifetime_{ lifetime },
				timerInterval_{ SecondsF{ interval }, StartImmediately::Yes, GlobalClock::Get() }
			{
			}

			bool update(double t) override
			{
				if (timerInterval_.reachedZero())
				{
					timerInterval_.restart();
					f_();
				}

				return t < lifetime_;
			}

			std::function<void()> f_;
			double lifetime_;
			Timer timerInterval_;
		};

		struct StarEffect : IEffect
		{
			StarEffect()
				:
				pos_{ RandomVec2(RectF{ 8, 14 + 4, SceneWidth - 16, 32 }) },
				lifetime_{ Random(0.3, 0.7) },
				textureName_{ Sample({ U"Star1", U"Star2" })}
			{
			}

			bool update(double t) override
			{
				const auto tex = SpriteSheet::GetFrame(TextureAsset(textureName_), 6, SecondsF{ lifetime_ }, t);
				tex.drawAt(pos_, AlphaF(0.7 * Periodic::Sine0_1(lifetime_ * 0.18, t)));

				return t < lifetime_;
			}

			Vec2 pos_;
			double lifetime_;
			String textureName_;
		};

		double RandomX()
		{
			return Random(16.0, SceneWidth - 16.0);
		}

		double PosRight()
		{
			return SceneWidth + 16;
		}

		double PosLeft()
		{
			return -16;
		}

		double ParseX(const String& text)
		{
			if (text == U"random")
			{
				return RandomX();
			}
			else if (text == U"right")
			{
				return PosRight();
			}
			else if (text == U"left")
			{
				return PosLeft();
			}

			return ParseFloat<double>(text);
		}

		double RandomY()
		{
			return Random(16.0, SceneHeight - 16.0);
		}

		double PosBottom()
		{
			return SceneHeight + 16;
		}

		double PosTop()
		{
			return -16;
		}

		double ParseY(const String& text)
		{
			if (text == U"random")
			{
				return RandomY();
			}
			else if (text == U"bottom")
			{
				return PosBottom();
			}
			else if (text == U"top")
			{
				return PosTop();
			}

			return ParseFloat<double>(text);
		}
	}


	MainScene::MainScene(const InitData& init)
		:
		IScene{ init },
		obj_{ getData().input },
		handExistsPrevFrame_{ false },
		time_{ StartImmediately::Yes, GlobalClock::Get() },
		timerSpawnEnemy_{ 5s, StartImmediately::Yes, GlobalClock::Get() },
		timePlayerDead_{ StartImmediately::No, GlobalClock::Get() },
		timerShake_{ 0.4s, StartImmediately::No, GlobalClock::Get() },
		timerShakeLong_{ 5s, StartImmediately::No, GlobalClock::Get() },
		scoreRateRaw_{ ScoreRateMin },
		timeIncrScoreRate_{ StartImmediately::No, GlobalClock::Get() },
		timerScoreRateGaugeDecr_{ ScoreRateDecrTime, StartImmediately::Yes, GlobalClock::Get() },
		eventList_{ obj_ },
		timeBgDarkOverlayAlpha_{ StartImmediately::No, GlobalClock::Get() },
		timeBgRain_{ StartImmediately::No, GlobalClock::Get() },
		timeStageTitle_{ StartImmediately::No, GlobalClock::Get() },
		timerStar_{ 8s, StartImmediately::No, GlobalClock::Get() },
		stage_{ 0 },
		timerMsg_{ 4.5s, StartImmediately::No, GlobalClock::Get() },
		msgRange_{},
		currentMsg_{},
		bgm_{ 1 },
		timerSe_{ 0.08s, StartImmediately::Yes }
	{
		eventList_.load(RES(StageEventFilePath[1]));
	}

	void MainScene::update()
	{
		if (timerShakeLong_.reachedZero())
		{
			timerShakeLong_.reset();
			AudioAsset(U"Quake").stop(3s);
		}

		// BGMフェードアウト
		if (currentBgm_().isPlaying() && bgm_ == 0)
		{
			currentBgm_().stop(2s);
		}

		// シーンエンド
		if (timeEndScene_ > 2s + 4.5s)
		{
			changeScene(U"EndingScene", 0s);
			return;
		}

		if (timeEndScene_ > 2s) return;

		// ポーズのためのカスタムクロック
		auto clock = GlobalClock::Get();

		// ポーズ切替
		if (getData().input.pause().down())
		{
			if (clock->isPaused())
			{
				clock->start();
				obj_.effect.resume();
				obj_.bgEffect.resume();

				if (currentBgm_().isPaused())
				{
					currentBgm_().play(MixBus1);
				}
			}
			else
			{
				clock->pause();
				obj_.effect.pause();
				obj_.bgEffect.pause();

				AudioAsset(U"Select").playOneShot();

				currentBgm_().pause();
			}
		}

		// ポーズ中はシーンを更新しない
		if (clock->isPaused()) return;


		// 各オブジェクトの更新…

		if (not timeEndScene_.isStarted())
		{
			obj_.player.update();
		}

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
		if (not timeEndScene_.isStarted())
		{
			checkCollision_();
		}

		// 期限切れの Hand を破棄
		obj_.hands.remove_if([](const auto& hand) { return not hand->isAlive(); });

		// 期限切れの Enemy のなかに HandE がある？
		for (const auto& enemy : obj_.enemies)
		{
			if (enemy->type() == EnemyType::HandE && not enemy->isAlive())
			{
				getData().endingType = 0;
				timeEndScene_.start();
				AudioAsset(U"NoiseFade").playOneShot();
			}
		}
		
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
		while (eventList_.hasActiveEvent(time_.sF()))
		{
			const auto& row = eventList_.getRow();
			doEvent_(row);

			eventList_.next();
		}

		// NightSky
		if (timeNightSky_.isRunning())
		{
			if (timerStar_.reachedZero())
			{
				timerStar_.restart(SecondsF{ Random(0.3, 2.0) });
				obj_.bgEffect.add<StarEffect>();
			}
		}

		// Msg
		if (timerMsg_.reachedZero())
		{
			++currentMsg_;
			if (currentMsg_ > msgRange_.second)
			{
				timerMsg_.reset();
			}
			else
			{
				timerMsg_.restart();
			}
		}

		//
		handExistsPrevFrame_ = not obj_.hands.isEmpty();
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
		obj_.bgEffect.update();

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
		drawStatusBG_();
		drawKarma_();
		drawScore_();

		drawMsg_();

		// ゲームオーバーへ移行直前のフェードアウト
		if (timePlayerDead_ > 2.0s)
		{
			constexpr double FadeTime = 2.0;
			const double t = Clamp((timePlayerDead_.sF() - 2.0) / FadeTime, 0.0, 1.0);
			SceneRect.draw(ColorF{ Theme::Black, t });
		}

		// エンディングへ移行直前のフェードアウト
		if (timeEndScene_ > 2.0s)
		{
			constexpr double FadeTime = 4.0;
			const double t = Clamp((timeEndScene_.sF() - 2.0) / FadeTime, 0.0, 1.0);
			SceneRect.draw(ColorF{ Theme::White, t });
		}

		// ポーズ
		if (paused)
		{
			SceneRect.draw(ColorF{ Theme::Black, 0.8 });
			FontAsset(U"H68Thin")(U"PAUSE").drawAt(SceneCenter, Theme::White);
		}
	}

	void MainScene::updateScoreRate_()
	{
		if (not handExistsPrevFrame_ && not obj_.hands.isEmpty())
		{
			// このフレームで Hand が生成されたのでゲージを少し維持する
			addScoreRateGauge_(ScoreRateGaugeIncrOnCallHand);
		}
		else if (not obj_.hands.isEmpty())
		{
			// Hand が存在するのでスコアレートを加算する...

			if (not timeIncrScoreRate_.isRunning()) timeIncrScoreRate_.restart();

			// 加算する倍率
			const double rateIncr = 16.0 * EaseInSine(Clamp(timeIncrScoreRate_.sF(), 0.0, 4.0) / 4.0) * Scene::DeltaTime();
			scoreRateRaw_ = Clamp(scoreRateRaw_ + rateIncr, ScoreRateMin, ScoreRateMax);
		}
		else
		{
			// Hand が存在しないのでスコアレートを減らす...

			timeIncrScoreRate_.reset();

			if (timerScoreRateGaugeDecr_.reachedZero())
			{
				timerScoreRateGaugeDecr_.restart(ScoreRateDecrTime);
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
					enemy->damage(20.0 * Scene::DeltaTime());

					if (not enemy->isAlive())
					{
						if (enemy->type() == EnemyType::HandE)
						{
							getData().endingType = 0;
							timeEndScene_.start();
							AudioAsset(U"NoiseFade").playOneShot();
							break;
						}

						// 敵を撃破したのでスコアを加算する
						addScore_(EnemyScore(enemy->type()) * scoreRate_());

						// 敵を撃破したのでスコアレートが少し増える
						scoreRateRaw_ = Clamp(scoreRateRaw_ + 0.1, ScoreRateMin, ScoreRateMax);

						// スコアレート減少タイマーの残り時間を加算
						addScoreRateGauge_(ScoreRateGaugeIncrOnDestroyEnemy);

						// 敵の撃破後にお金が散らばる
						if (IsEnemy(enemy->type()))
						{
							for ([[maybe_unused]] int iMoney : step(EnemyCoins(enemy->type())))
							{
								obj_.items.emplace_back(MakeItem<ItemMoney, ItemType::Money>(obj_.effect, enemy->pos()));
							}
						}

						// その場に倍率エフェクト
						if (scoreRate_() > 1.99)
						{
							obj_.effect.add<ScoreRateEffect>(enemy->pos(), scoreRate_());
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
				if (not item->enableCollision()) continue;

				if (obj_.player.collision().intersects(item->collision()) ||
					obj_.player.collisionAirplane().intersects(item->collision()) ||
					(not obj_.hands.isEmpty() && obj_.hands[0]->collision().intersects(item->collision())))
				{
					switch (item->type())
					{
					case ItemType::Money:
						obj_.player.addKarma(Player::KarmaRecoveryOnGetMoney);
						break;
					}

					// アイテムを取得したのでスコアを加算する
					addScore_(ItemScore(item->type()) * scoreRate_());

					// スコアレート減少タイマーの残り時間を加算
					addScoreRateGauge_(ScoreRateGaugeIncrOnGetMoney);

					// その場に倍率エフェクト
					if (scoreRate_() > 1.99)
					{
						obj_.effect.add<ScoreRateEffect>(item->pos(), scoreRate_());
					}

					item->kill();

					// 同時再生数が多くなりすぎないよう制限
					if (timerSe_.reachedZero())
					{
						AudioAsset(U"Coin").playOneShot();
						timerSe_.restart();
					}
				}
			}

			// 衝突判定 - Player vs Enemy
			for (auto& enemy : obj_.enemies)
			{
#ifdef DEBUG_MODE
				if (KeyI.pressed()) break; // [DEBUG]
#endif

				if (not obj_.player.isInvincible() && obj_.player.collision().intersects(enemy->collision()))
				{
					shake_();

					AudioAsset(U"Damage").playOneShot();

					if (enemy->type() == EnemyType::Bullet2 || enemy->type() == EnemyType::HandE)
					{
						timeEndScene_.start();
						getData().endingType = 1;
						AudioAsset(U"NoiseFade").playOneShot();
						break;
					}

					obj_.player.damage(20.0);

					// このダメージでプレイヤーがしんでしまった
					if (not obj_.player.isAlive())
					{
						timePlayerDead_.start();

						currentBgm_().stop(2s);
						bgm_ = 0;

						break;
					}

					// ダメージを受けるとスコアレートが 2/3 になる
					scoreRateRaw_ = Clamp(scoreRateRaw_ * 2 / 3, ScoreRateMin, ScoreRateMax);
				}
			}
		}
	}

	void MainScene::drawBG_() const
	{
		const ScopedRenderStates2D sampler{ SamplerState::RepeatNearest };

		const double nightAlpha = Clamp(timeNightSky_.sF() / 5.0, 0.0, 1.0);

		TextureAsset(U"BgMountain2")
			.mapped(640, 64)
			.draw(Arg::bottomLeft = Vec2{ -(static_cast<int>(time_.sF() * 12.0) % 320), SceneHeight }, ColorF{ Palette::White, 0.5 + 0.35 * nightAlpha });

		TextureAsset(U"BgMountain")
			.mapped(640, 64)
			.draw(Arg::bottomLeft = Vec2{ -(static_cast<int>(time_.sF() * 20.0) % 320), SceneHeight }, ColorF{ Palette::White });

		TextureAsset(U"BgMountainNight")
			.mapped(640, 64)
			.draw(Arg::bottomLeft = Vec2{ -(static_cast<int>(time_.sF() * 20.0) % 320), SceneHeight }, ColorF{ Palette::White, nightAlpha });

		TextureAsset(U"BgTree")
			.mapped(400, 32)
			.mirrored(true)
			.draw(Arg::bottomLeft = Vec2{ -(static_cast<int>(time_.sF() * 110.0) % 200), SceneHeight + 2 }, AlphaF(0.5));

		TextureAsset(U"BgTree")
			.mapped(400, 32)
			.draw(Arg::bottomLeft = Vec2{ -(static_cast<int>(time_.sF() * 140.0) % 200), SceneHeight + 4 }, AlphaF(1));

		// Dark
		const double bgDarkOverlayAlpha = 0.3 * Clamp(timeBgDarkOverlayAlpha_.sF() / 5.0, 0.0, 1.0);
		SceneRect.draw(ColorF{ Theme::Black, bgDarkOverlayAlpha });

		// Rain
		if (timeBgRain_.isRunning())
		{
			for ([[maybe_unused]] int iRain : step(24))
			{
				Line{ RandomVec2(SceneRect), Arg::angle = 45_deg, Random(6.0, 9.0) }
				.draw(ColorF{ Theme::White, Random(0.7, 1.0) });
			}
		}

		// NightSky
		if (timeNightSky_.isRunning())
		{
			const double alpha = 0.50 * Clamp(timeNightSky_.sF() / 7.0, 0.0, 1.0);
			TextureAsset(U"NightSky").draw(0, 14, AlphaF(alpha));

			const double starsAlpha = 0.95 * Clamp(timeNightSky_.sF() / 14.0, 0.0, 1.0);

			TextureAsset(U"Stars1")
				.mapped(640, 64)
				.draw(1.0 * -(static_cast<int>(time_.sF() * 1.2) % 320), 14.0, AlphaF(starsAlpha * Periodic::Sine0_1(SecondsF{ 0.1 + Random(0.1) } * (0.3 + 0.7 * Periodic::Sine0_1(3s)), time_.sF())));

			TextureAsset(U"Stars2")
				.mapped(640, 64)
				.draw(1.0 * -(static_cast<int>(time_.sF() * 0.9) % 320), 14.0, AlphaF(starsAlpha * Periodic::Jump0_1(SecondsF{ 0.18 + Random(0.1) } * (0.3 + 0.7 * Periodic::Sine0_1(5s)), time_.sF())));

		}
	}

	void MainScene::drawStageTitle_() const
	{
		constexpr double FadeTime = 0.6;
		constexpr double FadeOutWait = 2.8;

		if (timeStageTitle_.isRunning() && timeStageTitle_.sF() < FadeTime + FadeOutWait)
		{
			const double t = timeStageTitle_.sF();
			const double x = SceneCenter.x + 200 * Clamp(EaseInCubic(1.0 - t / FadeTime), 0.0, 1.0);
			const double xOut = (t > FadeOutWait) ? 200 * EaseInCubic((t - FadeOutWait) / FadeTime) : 0.0;

			double h = Clamp(t / FadeTime, 0.0, 1.0);
			if (t > FadeOutWait) h = 1.0 - Clamp((t - FadeOutWait) / FadeTime, 0.0, 1.0);

			const auto stageTextFunc = [](int stage) {
				switch (stage)
				{
				case 1: return U"AREA 1 - CLEAR SKY "_sv;
				case 2: return U"AREA 2 - TEARFUL RAIN"_sv;
				case 3: return U"AREA 3 - TO NOSTALGIC PLACE"_sv;
				}
				return U""_sv;
				};
			const auto stageText = stageTextFunc(stage_);

			RectF{ Arg::center = Vec2{ SceneWidth / 2, SceneHeight - 6 }, SceneWidth, 9 * h }.draw(Theme::White);
			FontAsset(U"H68Thin")(stageText).drawAt(x - xOut + 1, SceneHeight - 7 + 1, Theme::Lighter);
			FontAsset(U"H68Thin")(stageText).drawAt(x - xOut + 0, SceneHeight - 7 + 0, Theme::Black);
		};
	}

	void MainScene::drawStatusBG_() const
	{
		// ステータスエリアの背景は常に白
		Rect{ 0, 0, SceneWidth, 14 }.draw(Theme::White);
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
		int gaugeWidth = static_cast<int>(42 * karma / 100.0);
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
			Vec2 penPos{ 74, 3 };
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
			const auto rateRect = DrawScoreRate(Vec2{ SceneWidth - 12, 6 + 0 }, scoreRate_(), Theme::Black);

			// 倍率ゲージ
			// ゲージがなくなると倍率が減っていく感じ
			const double scoreRateGauge0_1 = Clamp((timerScoreRateGaugeDecr_.sF() - ScoreRateDecrTime.count()) / (ScoreRateDecrTimeMax.count() - ScoreRateDecrTime.count()), 0.0, 1.0);
			Line{ rateRect.bottomCenter().movedBy(-8, 2), rateRect.bottomCenter().movedBy(8, 2) }.draw(2.0, Theme::Lighter);
			Line{ rateRect.bottomCenter().movedBy(-8, 2), rateRect.bottomCenter().movedBy(-8 + 16 * scoreRateGauge0_1, 2) }.draw(2.0, Theme::Black);

		}
	}

	void MainScene::drawMsg_() const
	{
		if (timerMsg_.isRunning() && timerMsg_ > 1.5s)
		{
			RectF{ Arg::center = SceneCenter, SizeF{ SceneWidth, 32 } }.draw(ColorF{ Theme::White, 0.5 });

			FontAsset(U"Sub")(Msg::Data[currentMsg_]).drawAt(SceneCenter, ColorF{ Theme::Black, 0.7 + 0.3 * Periodic::Square0_1(0.01s) });
		}
	}

	void MainScene::shake_()
	{
		timerShake_.restart(0.4s);
	}

	double MainScene::scoreRate_() const
	{
		return static_cast<int>(scoreRateRaw_ * 10) / 10.0;
	}

	void MainScene::addScore_(double score)
	{
		getData().score += static_cast<int>(score / 10) * 10;
	}

	void MainScene::addScoreRateGauge_(const Duration& gaugeAdd)
	{
		const auto timerRemain = Clamp(timerScoreRateGaugeDecr_.remaining() + gaugeAdd, ScoreRateDecrTime, ScoreRateDecrTimeMax);
		timerScoreRateGaugeDecr_.restart(timerRemain);
	}

	void MainScene::doEvent_(const Array<String>& eventCsvRow)
	{
		const String textType = eventCsvRow[1].trimmed().lowercased();
		const String textX = eventCsvRow[2].trimmed().lowercased();
		const String textY = eventCsvRow[3].trimmed().lowercased();

		const Vec2 pos{ ParseX(textX), ParseY(textY) };

		if (textType == U"bird1")
		{
			const double speedScale = ParseOr<double>(eventCsvRow[4], 1.0);
			const double accel = ParseOr<double>(eventCsvRow[5], 0.0);
			obj_.enemies.emplace_back(MakeEnemy<Bird1, EnemyType::Bird1>(obj_, pos, speedScale, accel));
		}
		else if (textType == U"bird2")
		{
			const double speedScale = ParseOr<double>(eventCsvRow[4], 1.0);
			obj_.enemies.emplace_back(MakeEnemy<Bird2, EnemyType::Bird2>(obj_, pos, speedScale));
		}
		else if (textType == U"bird3")
		{
			obj_.enemies.emplace_back(MakeEnemy<Bird3, EnemyType::Bird3>(obj_, pos));
		}
		else if (textType == U"bird4")
		{
			obj_.enemies.emplace_back(MakeEnemy<Bird4, EnemyType::Bird4>(obj_, pos));
		}
		else if (textType == U"birdb1")
		{
			const double speedY0 = ParseOr<double>(eventCsvRow[4], 200.0);
			obj_.enemies.emplace_back(MakeEnemy<BirdB1, EnemyType::BirdB1>(obj_, pos, speedY0));
		}
		else if (textType == U"jellyfish1")
		{
			obj_.enemies.emplace_back(MakeEnemy<JellyFish1, EnemyType::JellyFish1>(obj_, pos));
		}
		else if (textType == U"bat1")
		{
			const double y = ParseOr<double>(eventCsvRow[4], 72.0);
			obj_.enemies.emplace_back(MakeEnemy<Bat1, EnemyType::Bat1>(obj_, pos, y));
		}
		else if (textType == U"hand")
		{
			obj_.enemies.emplace_back(MakeEnemy<HandE, EnemyType::HandE>(obj_, pos));
		}
		else if (textType == U"genbird1")
		{
			const double lifetime = ParseFloat<double>(eventCsvRow[4]);
			const double interval = ParseFloat<double>(eventCsvRow[5]);
			const double speedScale = ParseOr<double>(eventCsvRow[6], 1.0);
			const double accel = ParseOr<double>(eventCsvRow[7], 0.0);
			obj_.effect.add<GenerateEnemies>([&, speedScale, accel, textX, textY]() {
				const Vec2 pos{ ParseX(textX), ParseY(textY) };
				obj_.enemies.emplace_back(MakeEnemy<Bird1, EnemyType::Bird1>(obj_, pos, speedScale, accel));
				}, lifetime, interval);
		}
		else if (textType == U"genbird2")
		{
			const double lifetime = ParseFloat<double>(eventCsvRow[4]);
			const double interval = ParseFloat<double>(eventCsvRow[5]);
			const double speedScale = ParseFloat<double>(eventCsvRow[6]);
			obj_.effect.add<GenerateEnemies>([&, speedScale, textX, textY]() {
				const Vec2 pos{ ParseX(textX), ParseY(textY) };
				obj_.enemies.emplace_back(MakeEnemy<Bird2, EnemyType::Bird2>(obj_, pos, speedScale));
				}, lifetime, interval);
		}
		else if (textType == U"genjellyfish1")
		{
			const double lifetime = ParseFloat<double>(eventCsvRow[4]);
			const double interval = ParseFloat<double>(eventCsvRow[5]);
			obj_.effect.add<GenerateEnemies>([&, textX, textY]() {
				const Vec2 pos{ Random(SceneWidth / 2, SceneWidth), ParseY(textY)};
				obj_.enemies.emplace_back(MakeEnemy<JellyFish1, EnemyType::JellyFish1>(obj_, pos));
				}, lifetime, interval);
		}
		else if (textType == U"bgdark1")
		{
			timeBgDarkOverlayAlpha_.restart();
		}
		else if (textType == U"bgdarkend")
		{
			timeBgDarkOverlayAlpha_.reset();
		}
		else if (textType == U"bgrain1")
		{
			timeBgRain_.restart();
		}
		else if (textType == U"bgrainend")
		{
			timeBgRain_.reset();
		}
		else if (textType == U"nightsky1")
		{
			timeNightSky_.restart();
			timerStar_.restart();
		}
		else if (textType == U"stagetitle")
		{
			stage_ = ParseInt<int>(eventCsvRow[4]);
			timeStageTitle_.restart();

#ifdef DEBUG_MODE
			// [DEBUG]
			//Print << time_.sF();
#endif
		}
		else if (textType == U"goto")
		{
			StringView labelDest = eventCsvRow[4].starts_with(U':') ? eventCsvRow[4].substrView(1) : eventCsvRow[4];
			eventList_.gotoLabel(labelDest);
		}
		else if (textType == U"msg")
		{
			const int msgRange0 = ParseInt<int>(eventCsvRow[4]);
			const int msgRange1 = ParseInt<int>(eventCsvRow[5]);
			msgRange_ = std::make_pair(msgRange0, msgRange1);
			currentMsg_ = msgRange0;
			timerMsg_.restart();
		}
		else if (textType == U"bgm")
		{
			const int bgmNum = ParseOr<int>(eventCsvRow[4], 0);

			currentBgm_().stop(3s);
			bgm_ = bgmNum;
			currentBgm_().play(3s, MixBus1);

		}
		else if (textType == U"quake")
		{
			AudioAsset(U"Quake").setLoop(true);
			AudioAsset(U"Quake").play(MixBus1, 0.8s);
			timerShake_.restart(8s);
			timerShakeLong_.restart();
		}
	}

	AudioAsset MainScene::currentBgm_()
	{
		return AudioAsset(Format(U"Bgm", bgm_));
	}
}
