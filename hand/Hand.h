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

	private:
		Vec2 pos_;

		Stopwatch time_;

		bool killed_;
	};

	using HandPtr = std::unique_ptr<Hand>;
}
