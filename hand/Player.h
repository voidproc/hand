#pragma once

#include "Hand.h"

namespace hand
{
	class Player
	{
	public:
		static inline constexpr double KarmaMax = 100;
		static inline constexpr double DefaultKarma = 75;
		static inline constexpr double KarmaRecoveryPerSec = 0.25;
		static inline constexpr double KarmaRecoveryOnGetMoney = 4;

		static inline constexpr double KarmaCostOnAction = 1.0;// 9.5;
		static inline constexpr double KarmaCostOnActionPerSec = 12.0;
		static inline constexpr double KarmaEmptyThreshold = 2.0;

		static inline constexpr double TimeDamageInvSec = 2.0;
		static inline constexpr double TimeKnockBackSec = 0.55;

	public:
		Player(Array<HandPtr>& hands);

		void update();

		void draw() const;

		RectF collision() const;

		bool isAlive() const;

		double karma() const;

		void addKarma(double amount);

		void damage(double damageAmount);

		bool isInvincible() const;

	private:
		void appear_();

		Array<HandPtr>& hands_;

		Vec2 pos_;

		Vec2 vel_;

		double karma_;

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
