#pragma once

#include "Hand.h"

namespace hand
{
	class Player
	{
	public:
		static inline constexpr int LifeMax = 10;
		static inline constexpr int DefaultLife = 10;

		static inline constexpr double KarmaMax = 100;
		static inline constexpr double DefaultKarma = 50;
		static inline constexpr double KarmaRecoveryPerSec = 0.3;
		static inline constexpr double KarmaRecoveryOnGetMoney = 4;

		static inline constexpr double KarmaCostOnAction = 12;

	public:
		Player(Array<HandPtr>& hands);

		void update();

		void draw() const;

		RectF collision() const;

		int life() const;

		int karma() const;

		void addKarma(double amount);

		void damage(int damageAmount);

	private:
		void appear_();

		Array<HandPtr>& hands_;

		Vec2 pos_;

		Vec2 vel_;

		int life_;

		double karma_;

		// シーン開始時に画面外から登場する用
		Stopwatch timeAppear_;

		// 煙用
		Timer timerSmoke_;

		// カルマ回復用
		Timer timerRecoverKarma_;

		// 各種エフェクト用
		Effect effect_;
	};
}
