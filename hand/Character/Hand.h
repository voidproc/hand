#pragma once

namespace hand
{
	enum class HandDirection
	{
		None,
		Up,
		Down,
	};

	class Hand
	{
	public:
		Hand(const Vec2& pos, HandDirection dir);

		void update();

		void draw() const;

		bool isAlive() const;

		RectF collision() const;

		void kill();

	private:
		Vec2 pos_;

		HandDirection dir_;

		// Hand 生成時に決定する縦方向のスピード
		double ySpeed_;

		Stopwatch time_;

		bool killed_;
	};

	using HandPtr = std::unique_ptr<Hand>;
}
