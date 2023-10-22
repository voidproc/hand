#pragma once

namespace hand
{
	enum class EnemyType
	{
		Bird1,

		Bullet1,
	};

	class Enemy
	{
	public:
		Enemy(const Vec2& pos, EnemyType type);

		void update();

		void draw() const;

		bool isAlive() const;

		RectF collision() const;

		void damage(int damageAmount);

	private:
		Vec2 pos_;

		EnemyType type_;

		int life_;

		Stopwatch time_;
	};

	using EnemyPtr = std::unique_ptr<Enemy>;
}
