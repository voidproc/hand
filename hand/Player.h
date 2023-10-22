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

	private:
		void appear_();

		Array<HandPtr>& hands_;

		Vec2 pos_;

		Vec2 vel_;

		// シーン開始時に画面外から登場する用
		Stopwatch timeAppear_;

		// 煙用
		Timer timerSmoke_;

		// 各種エフェクト用
		Effect effect_;
	};
}
