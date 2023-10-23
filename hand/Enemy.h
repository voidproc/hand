#pragma once

namespace hand
{
	class Enemy
	{
	public:
		Enemy(Effect& effect, const Vec2& pos);

		virtual void update();

		virtual void draw() const;

		virtual RectF collision() const;

		bool isAlive() const;

		void damage(int damageAmount);

	protected:
		Effect& effect_;

		Vec2 pos_;

		int life_;

		Stopwatch time_;
	};

	class Bird1 : public Enemy
	{
	public:
		Bird1(Effect& effect, const Vec2& pos);

		void update() override;

		void draw() const override;

		RectF collision() const override;
	};

	class Bird2 : public Enemy
	{
	public:
		Bird2(Effect& effect, const Vec2& pos);

		void update() override;

		void draw() const override;

		RectF collision() const override;
	};

	using EnemyPtr = std::unique_ptr<Enemy>;
}
