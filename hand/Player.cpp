#include "Player.h"
#include "SceneSize.h"
#include "Theme.h"
#include "SpriteSheet.h"

namespace hand
{
	struct SmokeEffect : IEffect
	{
		SmokeEffect(const Vec2& pos, const Vec2& playerVel)
			:
			pos_{ pos.x, pos.y + Random(-2.0, 2.0) },
			playerVel_{ playerVel },
			lifetime_{Random(0.05, 0.2)}
		{
		}

		bool update(double t) override
		{
			const double t0_1 = t / lifetime_;
			const double alpha = Periodic::Square0_1(0.08s);

			Circle{ pos_.movedBy((-12 + playerVel_.x * 2.0) * EaseOutCubic(t0_1), 0), 2 + 3 * t0_1 }
				.draw(ColorF{ Theme::Lighter, alpha })
				.drawFrame(4.0 - 4.0 * t0_1, 0.0, ColorF{ Theme::Darker, alpha });

			return t < lifetime_;
		}

		Vec2 pos_;
		Vec2 playerVel_;
		double lifetime_;
	};


	Player::Player(Array<HandPtr>& hands)
		:
		hands_{ hands },
		pos_{},
		vel_{},
		karma_{ DefaultKarma },
		timeAppear_{ StartImmediately::No },
		timerSmoke_{ 1s, StartImmediately::Yes },
		timerRecoverKarma_{ 1s, StartImmediately::No },
		timerDamage_{ SecondsF{ TimeDamageInvSec }, StartImmediately::No },
		timeDead_{ StartImmediately::No },
		effect_{}
	{
		appear_();
	}

	void Player::update()
	{
		// 死んでる？
		if (timeDead_.isRunning())
		{
			vel_.setLength(0);
			return;
		}

		// ダメージ用タイマーのリセット
		if (timerDamage_.reachedZero())
		{
			timerDamage_.reset();
		}

		// 画面外から登場する
		if (timeAppear_.isRunning())
		{
			constexpr double TimeAppearSec = 1.5;

			if (timeAppear_.sF() < TimeAppearSec)
			{
				pos_.x = -24 + ((24 + 28) * EaseOutSine(timeAppear_.sF() / TimeAppearSec));
			}
			else
			{
				// 画面外からの登場終了
				timeAppear_.reset();

				// カルマ回復開始
				timerRecoverKarma_.start();
			}
		}

		// ふわふわ
		pos_.y += 8.0 * Periodic::Sine1_1(2.5s) * Scene::DeltaTime();

		// 入力に反応
		if (not timeAppear_.isRunning())
		{
			// 移動のためのキー入力

			Vec2 vel{};
			if (KeyLeft.pressed()) vel.x = -1;
			if (KeyRight.pressed()) vel.x = 1;
			if (KeyUp.pressed()) vel.y = -1;
			if (KeyDown.pressed()) vel.y = 1;
			vel.limitLengthSelf(1.0);

			// ダメージ中は入力無効
			if (timerDamage_.isRunning() && timerDamage_.sF() > TimeDamageInvSec - TimeKnockBackSec)
			{
				vel.setLength(0);
			}

			if (vel.length() > 1e-3)
			{
				vel_ += vel;
				vel_.limitLengthSelf(4.5);
			}
			else
			{
				// ボタンが押されてないので減速
				if (vel_.length() > 0.2)
				{
					vel_ -= vel_ * (60.0 / 16.0) * Scene::DeltaTime();
				}
			}

			// 移動
			pos_ += vel_ * (60.0 / 4.0) * Scene::DeltaTime();
			pos_.clamp(SceneRect.stretched(-20, -12, -12, -12));

			// 他の Hand が存在していないとき、カルマを一定量消費して Hand を生成できる
			if (KeySpace.down())
			{
				if (hands_.isEmpty() && karma_ >= KarmaEmptyThreshold)
				{
					karma_ = Clamp(karma_ - KarmaCostOnAction, 0.1, KarmaMax);
					hands_.emplace_back(std::make_unique<Hand>(pos_.movedBy(24, 0)));
				}
			}
		}

		// 飛行機から煙が出る
		if (timerSmoke_.reachedZero())
		{
			timerSmoke_.restart(SecondsF{ Random(0.1, 0.3) });
			effect_.add<SmokeEffect>(pos_.movedBy(-8, 8), vel_);
		}

		// 一定時間ごとにカルマが回復する
		if (timerRecoverKarma_.reachedZero())
		{
			timerRecoverKarma_.restart();
			karma_ = Clamp(karma_ + KarmaRecoveryPerSec, 0.0, KarmaMax);
		}
	}

	void Player::draw() const
	{
		// 死んでる？
		if (timeDead_.isRunning())
		{
			const Color color = Palette::White.lerp(Theme::Black, EaseInQuad(Clamp(timeDead_.sF() / 1.0, 0.0, 1.0)));
			TextureAsset(U"Airplane").drawAt(pos_.movedBy(0, 6), color);
			SpriteSheet::DrawAt(TextureAsset(U"Girl"), 2, pos_, color, 0.5s);
			return;
		}

		// 煙エフェクト
		effect_.update();

		// ステージ開始時の点滅
		const Color colorAppear = Palette::White.lerp(Theme::Darker, Periodic::Square0_1(0.1s));

		// 無敵時間中の点滅
		const Color colorInvinclble = (timerDamage_.sF() > TimeDamageInvSec - TimeKnockBackSec) ? ColorF{colorAppear} : AlphaF(0.7 - 0.6 * Periodic::Square0_1(0.12s));

		const Color color = timeAppear_.isRunning() ? colorAppear : (timerDamage_.isRunning() ? colorInvinclble : Palette::White);

		// 紙飛行機
		TextureAsset(U"Airplane").drawAt(pos_.movedBy(0, 6), color);

		// Girl
		SpriteSheet::DrawAt(TextureAsset(U"Girl"), 2, pos_, color, 0.2s);

		// [DEBUG] 当たり判定
		//collision().drawFrame(1, 0, Palette::Magenta.withAlpha(128));
	}

	RectF Player::collision() const
	{
		return RectF{ Arg::center = pos_, 12.0 };
	}

	bool Player::isAlive() const
	{
		return not timeDead_.isRunning();
	}

	double Player::karma() const
	{
		return karma_;
	}

	void Player::addKarma(double amount)
	{
		karma_ = Clamp(karma_ + amount, 0.1, KarmaMax);
	}

	void Player::damage(double damageAmount)
	{
		if (karma_ >= 0.1)
		{
			karma_ = Clamp(karma_ - damageAmount, 0.0, KarmaMax);

			vel_.x -= 8.0;

			if (karma_ < 1e-3)
			{
				// ゲームオーバー
				timerDamage_.reset();
				timeDead_.start();
			}
			else
			{
				timerDamage_.restart();
			}
		}
	}

	bool Player::isInvincible() const
	{
		return timerDamage_.isRunning();
	}

	void Player::appear_()
	{
		pos_ = SceneRect.leftCenter().movedBy(-24, 0);
		vel_.set(0, 0);
		timeAppear_.restart();
	}
}
