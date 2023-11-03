#include "Player.h"
#include "InputDevice.h"
#include "SceneSize.h"
#include "Theme.h"
#include "SpriteSheet.h"

namespace hand
{
	namespace
	{
		struct SmokeEffect : IEffect
		{
			SmokeEffect(const Vec2& pos, const Vec2& playerVel)
				:
				pos_{ pos.x, pos.y + Random(-2.0, 2.0) },
				playerVel_{ playerVel },
				lifetime_{ Random(0.05, 0.2) }
			{
			}

			bool update(double t) override
			{
				const double t0_1 = t / lifetime_;
				const double alpha = Periodic::Square0_1(0.08s, t);

				Circle{ pos_.movedBy((-12 + playerVel_.x * 2.0) * EaseOutCubic(t0_1), 0), 2 + 3 * t0_1 }
					.draw(ColorF{ Theme::Lighter, alpha })
					.drawFrame(4.0 - 4.0 * t0_1, 0.0, ColorF{ Theme::Darker, alpha });

				return t < lifetime_;
			}

			Vec2 pos_;
			Vec2 playerVel_;
			double lifetime_;
		};

		struct HandEffect : IEffect
		{
			HandEffect(const Vec2& pos, HandDirection dir)
				: pos_{ pos }, dir_{ dir }, ySpeed_{ 0 }
			{
				if (dir == HandDirection::Up)
				{
					pos_.y -= 16.0;
					ySpeed_ = 40.0;
				}
				else if (dir == HandDirection::Down)
				{
					pos_.y += 16.0;
					ySpeed_ = 40.0;
				}
			}

			bool update(double t) override
			{
				constexpr double Lifetime = 0.2;

				pos_.x += 60 * (1.0 - EaseOutSine(t / Lifetime)) * Scene::DeltaTime();
				pos_.y += ySpeed_ * Scene::DeltaTime();

				ySpeed_ = Max(Abs(ySpeed_) - 50.0 * 3 * Scene::DeltaTime(), 0.0) * Sign(ySpeed_);

				TextureAsset(U"Hand").drawAt(pos_, AlphaF(0.7 - 0.7 * t / Lifetime));

				return t < Lifetime;
			}

			Vec2 pos_;
			HandDirection dir_;
			double ySpeed_;
		};

		HandDirection GetHandDirectionByInput(InputDevice& input)
		{
			if (input.upPressed())
			{
				return HandDirection::Up;
			}

			if (input.downPressed())
			{
				return HandDirection::Down;
			}

			return HandDirection::None;
		}
	}

	Player::Player(InputDevice& input, Array<HandPtr>& hands)
		:
		input_{ input },
		hands_{ hands },
		pos_{},
		vel_{},
		karma_{ DefaultKarma },
		time_{ StartImmediately::Yes, GlobalClock::Get() },
		timeAppear_{ StartImmediately::No, GlobalClock::Get() },
		timerSmoke_{ 1s, StartImmediately::Yes, GlobalClock::Get() },
		timerRecoverKarma_{ 1s, StartImmediately::No, GlobalClock::Get() },
		timerDamage_{ SecondsF{ TimeDamageInvSec }, StartImmediately::No, GlobalClock::Get() },
		timeDead_{ StartImmediately::No, GlobalClock::Get() },
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
		pos_.y += 8.0 * Periodic::Sine1_1(2.5s, time_.sF()) * Scene::DeltaTime();

		// 入力に反応
		if (not timeAppear_.isRunning())
		{
			// 移動のためのキー入力

			Vec2 vel{};
			if (input_.leftPressed()) vel.x = -1;
			if (input_.rightPressed()) vel.x = 1;
			if (input_.upPressed()) vel.y = -1;
			if (input_.downPressed()) vel.y = 1;
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

			// Hand の生成
			// 他の Hand が存在していないとき、Hand を生成できる
			if (input_.action().down())
			{
				if (hands_.isEmpty() && karma_ >= KarmaEmptyThreshold)
				{
					karma_ = Clamp(karma_ - KarmaCostOnAction, 0.1, KarmaMax);

					const Vec2 handPos = pos_.movedBy(24, 0);
					const auto handDir = GetHandDirectionByInput(input_);
					hands_.emplace_back(std::make_unique<Hand>(handPos, handDir));

					// エフェクト
					effect_.add<HandEffect>(handPos, handDir);

					AudioAsset(U"HandShot").playOneShot();
				}
			}

			// カルマが切れるかボタンが離されたらすべての Hand を破棄
			if (not hands_.isEmpty() && (not input_.action().pressed() || karma_ < KarmaEmptyThreshold))
			{
				for (auto& hand : hands_)
				{
					hand->kill();
				}
			}

			// カルマがある限り、ボタンおしっぱで Hand を維持し続けられる
			if (not hands_.isEmpty())
			{
				karma_ = Clamp(karma_ - KarmaCostOnActionPerSec * Scene::DeltaTime(), 0.1, KarmaMax);
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
			SpriteSheet::DrawAt(TextureAsset(U"Girl"), 2, pos_, color, 0.5s, time_.sF());
			return;
		}

		// 煙エフェクト
		effect_.update();

		// ステージ開始時の点滅
		const Color colorAppear = Palette::White.lerp(Theme::Darker, Periodic::Square0_1(0.1s, time_.sF()));

		// 無敵時間中の点滅
		const Color colorInvincible = (timerDamage_.sF() > TimeDamageInvSec - TimeKnockBackSec) ? ColorF{colorAppear} : AlphaF(0.7 - 0.6 * Periodic::Square0_1(0.12s, time_.sF()));

		// 死にそう
		const Color colorDanger = Palette::White.lerp(Theme::Darker, Periodic::Square0_1(0.3s, time_.sF()));

		const Color color = timeAppear_.isRunning() ? colorAppear :
			(timerDamage_.isRunning() ? colorInvincible :
				((karma_ <= KarmaDanger) ? colorDanger : Palette::White));

		// 紙飛行機
		TextureAsset(U"Airplane").drawAt(pos_.movedBy(0, 6), color);

		// Girl
		SpriteSheet::DrawAt(TextureAsset(U"Girl"), 2, pos_, color, 0.2s, time_.sF());

		// "Player(矢印)"
		if (time_ < 3.5s)
		{
			TextureAsset(U"PlayerIndicator").drawAt(pos_.movedBy(0, -16), AlphaF(Periodic::Square0_1(0.6s, time_.sF())));
		}

		// [DEBUG] 当たり判定
		//collision().drawFrame(1, 0, Palette::Magenta.withAlpha(128));
		//collisionAirplane().drawFrame(1, 0, Palette::Magenta.withAlpha(128));
	}

	RectF Player::collision() const
	{
		return RectF{ Arg::center = pos_, 12.0 };
	}

	RectF Player::collisionAirplane() const
	{
		return RectF(Arg::center = pos_.movedBy(0, 6), SizeF{ 22, 8 });
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

	double Player::angleFrom(const Vec2 p) const
	{
		return Atan2(pos_.x - p.x, p.y - pos_.y);
	}

	const Vec2& Player::pos() const
	{
		return pos_;
	}

	void Player::appear_()
	{
		pos_ = SceneRect.leftCenter().movedBy(-24, 0);
		vel_.set(0, 0);
		timeAppear_.restart();
	}
}
