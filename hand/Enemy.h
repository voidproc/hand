#pragma once

namespace hand
{
	enum class EnemyType
	{
		Bird1,
		Bird2,
		Bird3,

		Bullet1,
		Bullet2,
		Bullet3,
	};

	bool IsEnemy(EnemyType type);

	bool IsBullet(EnemyType type);

	class Enemy;
	using EnemyPtr = std::unique_ptr<Enemy>;

	class Enemy
	{
	public:
		Enemy(EnemyType type, Effect& effect, Array<EnemyPtr>& enemies, const Vec2& pos);

		EnemyType type() const;

		virtual void update();

		virtual void draw() const;

		virtual RectF collision() const;

		virtual void explode();

		bool isAlive() const;

		void damage(int damageAmount);

		const Vec2& pos() const;

	private:
		EnemyType type_;

	protected:
		Effect& effect_;
		Array<EnemyPtr>& enemies_;

		Vec2 pos_;

		int life_;

		Stopwatch time_;
	};

	// 敵（鳥）
	// 直進する
	class Bird1 : public Enemy
	{
	public:
		Bird1(EnemyType type, Effect& effect, Array<EnemyPtr>& enemies, const Vec2& pos);

		void update() override;

		void draw() const override;

		RectF collision() const override;
	};

	// 敵（鳥）
	// ゆらゆら
	class Bird2 : public Enemy
	{
	public:
		Bird2(EnemyType type, Effect& effect, Array<EnemyPtr>& enemies, const Vec2& pos);

		void update() override;

		void draw() const override;

		RectF collision() const override;

	private:
		Timer timerFire_;
	};

	// 敵弾（小）
	// 直進する
	class Bullet1 : public Enemy
	{
	public:
		Bullet1(EnemyType type, Effect& effect, Array<EnemyPtr>& enemies, const Vec2& pos, const Vec2& vel);

		void update() override;

		void draw() const override;

		RectF collision() const override;

		void explode() override;

	private:
		Vec2 vel_;
	};

	template <class T, EnemyType E, class ... Args>
	EnemyPtr MakeEnemy(Args&& ... args)
	{
		return std::make_unique<T>(E, std::forward<Args>(args)...);
	}
}
