#pragma once

#include "Hand.h"

namespace hand
{
	class Player
	{
	public:
		Player(Array<HandPtr>& hands);

		void update();

		void draw() const;

		RectF collision() const;

		int life() const;

		int karma() const;

		void damage(int damageAmount);

	private:
		void appear_();

		Array<HandPtr>& hands_;

		Vec2 pos_;

		Vec2 vel_;

		int life_;

		int karma_;

		// シーン開始時に画面外から登場する用
		Stopwatch timeAppear_;

		// 煙用
		Timer timerSmoke_;

		// 各種エフェクト用
		Effect effect_;
	};
}
