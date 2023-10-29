#pragma once

#include "Hand.h"

namespace hand
{
	class InputDevice;

	class Player
	{
	public:
		static inline constexpr double KarmaMax = 100;
		static inline constexpr double KarmaDanger = 20;
		static inline constexpr double DefaultKarma = 75;
		static inline constexpr double KarmaRecoveryPerSec = 0.25;
		static inline constexpr double KarmaRecoveryOnGetMoney = 4;

		static inline constexpr double KarmaCostOnAction = 5.0;
		static inline constexpr double KarmaCostOnActionPerSec = 12.0;
		static inline constexpr double KarmaEmptyThreshold = 2.0;

		static inline constexpr double TimeDamageInvSec = 2.0;
		static inline constexpr double TimeKnockBackSec = 0.55;

	public:
		Player(InputDevice& input, Array<HandPtr>& hands);

		void update();

		void draw() const;

		RectF collision() const;

		RectF collisionAirplane() const;

		bool isAlive() const;

		double karma() const;

		void addKarma(double amount);

		void damage(double damageAmount);

		bool isInvincible() const;

		double angleFrom(const Vec2 p) const;

		const Vec2& pos() const;

	private:
		void appear_();

		InputDevice& input_;

		Array<HandPtr>& hands_;

		Vec2 pos_;

		Vec2 vel_;

		double karma_;

		Stopwatch time_;

		// シーン開始時に画面外から登場する用
		Stopwatch timeAppear_;

		// 煙用
		Timer timerSmoke_;

		// カルマ回復用
		Timer timerRecoverKarma_;

		// ダメージ用
		Timer timerDamage_;

		// カルマがなくなった
		Stopwatch timeDead_;

		// 各種エフェクト用
		Effect effect_;
	};
}
