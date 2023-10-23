﻿#include "Player.h"
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
			const int alpha = 255 * Periodic::Square0_1(0.08s);

			Circle{ pos_.movedBy((-12 + playerVel_.x * 2.0) * EaseOutCubic(t0_1), 0), 2 + 3 * t0_1 }
				.draw(Theme::Lighter.withAlpha(alpha))
				.drawFrame(4.0 - 4.0 * t0_1, 0.0, Theme::Darker.withAlpha(alpha));

			return t < lifetime_;
		}

		Vec2 pos_;
		Vec2 playerVel_;
		double lifetime_;
	};


	namespace
	{
		constexpr int LifeMax = 10;
		constexpr int DefaultLife = 10;

		constexpr int KarmaMax = 100;
		constexpr int DefaultKarma = 50;
		constexpr int KarmaRecoveryPerSec = 2;

		constexpr int KarmaCostOnAction = 12;
	}

	Player::Player(Array<HandPtr>& hands)
		:
		hands_{ hands },
		pos_{},
		vel_{},
		life_{ DefaultLife },
		karma_{ DefaultKarma },
		timeAppear_{ StartImmediately::No },
		timerSmoke_{ 1s, StartImmediately::Yes },
		timerRecoverKarma_{ 1s, StartImmediately::No },
		effect_{}
	{
		appear_();
	}

	void Player::update()
	{
		// 画面外から登場する
		if (timeAppear_.isRunning())
		{
			if (timeAppear_ < 1.5s)
			{
				pos_.x = -24 + ((24 + 28) * EaseOutSine(timeAppear_.sF() / 1.5));
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
			// ミリ残しでも OK
			if (KeySpace.down())
			{
				if (hands_.isEmpty() && karma_ > 0)
				{
					karma_ = Clamp(karma_ - KarmaCostOnAction, 0, KarmaMax);
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
			karma_ = Clamp(karma_ + KarmaRecoveryPerSec, 0, KarmaMax);
		}
	}

	void Player::draw() const
	{
		// 煙エフェクト
		effect_.update();

		const Color color = (timeAppear_.isRunning()) ? Palette::White.lerp(Theme::Lighter, Periodic::Square0_1(0.1s)) : Palette::White;

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

	int Player::life() const
	{
		return life_;
	}

	int Player::karma() const
	{
		return karma_;
	}

	void Player::damage(int damageAmount)
	{
		if (life_ > 0)
		{
			life_ -= damageAmount;

			if (life_ <= 0)
			{
				// ゲームオーバー？
				//...
			}
		}
	}

	void Player::appear_()
	{
		pos_ = SceneRect.leftCenter().movedBy(-24, 0);
		vel_.set(0, 0);
		timeAppear_.restart();
	}
}
