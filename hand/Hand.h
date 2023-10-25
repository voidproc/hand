#pragma once

namespace hand
{
	class Hand
	{
	public:
		Hand(const Vec2& pos);

		void update();

		void draw() const;

		bool isAlive() const;

		RectF collision() const;

		void kill();

		// Hand の生存時間に応じてスコアにかかる倍率が高まっていく
		double getScoreRate() const;

	private:
		Vec2 pos_;

		Stopwatch time_;

		bool killed_;
	};

	using HandPtr = std::unique_ptr<Hand>;
}
