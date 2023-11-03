#pragma once

namespace hand
{
	enum class EnemyType
	{
		Bird1,
		Bird2,
		Bird3,
		Bird4,
		BirdB1,
		JellyFish1,
		Bat1,
		Bat2,
		HandE,

		Bullet1,
		Bullet2,
		Bullet3,
	};

	bool IsEnemy(EnemyType type);

	bool IsBullet(EnemyType type);

	int EnemyScore(EnemyType type);

	int EnemyCoins(EnemyType type);

	class Enemy;
	using EnemyPtr = std::unique_ptr<Enemy>;

	struct Objects;

	class Enemy
	{
	public:
		Enemy(EnemyType type, Objects& obj, const Vec2& pos);

		EnemyType type() const;

		virtual void update();

		virtual void draw() const;

		virtual RectF collision() const;

		virtual void explode();

		bool isAlive() const;

		void damage(double damageAmount);

		const Vec2& pos() const;

		Vec2 drawPos() const;

	private:
		EnemyType type_;

	protected:
		Objects& obj_;

		Vec2 pos_;

		double life_;

		Stopwatch time_;

		Timer timerDamaged_;
	};

	// 敵（鳥）
	// 直進する
	class Bird1 : public Enemy
	{
	public:
		Bird1(EnemyType type, Objects& obj, const Vec2& pos, double speedScale = 1.0, double accel = 0.0);

		void update() override;

		void draw() const override;

		RectF collision() const override;

	private:
		Vec2 vel_;
		double speedScale_;
		double accel_;
	};

	// 敵（鳥）
	// ゆらゆら
	class Bird2 : public Enemy
	{
	public:
		Bird2(EnemyType type, Objects& obj, const Vec2& pos, double speedScale = 1.0);

		void update() override;

		void draw() const override;

		RectF collision() const override;

	private:
		double speedScale_;
	};

	// 画面右側で止まって弾を撃ち、Uターンする
	class Bird3 : public Enemy
	{
	public:
		Bird3(EnemyType type, Objects& obj, const Vec2& pos);

		void update() override;

		void draw() const override;

		RectF collision() const override;

	private:
		SimpleAnimation anim_;
		Timer timerFire_;
	};

	class Bird4 : public Enemy
	{
	public:
		Bird4(EnemyType type, Objects& obj, const Vec2& pos);

		void update() override;

		void draw() const override;

		RectF collision() const override;

	private:
		SimpleAnimation anim_;
		Timer timerFire_;
	};

	// 敵（鳥B）
	// 急降下し、プレイヤーと Y 座標が合うか指定の Y 座標に達したら直進する
	class BirdB1 : public Enemy
	{
	public:
		BirdB1(EnemyType type, Objects& obj, const Vec2& pos, double speedY0);

		void update() override;

		void draw() const override;

		RectF collision() const override;

	private:
		SimpleAnimation anim1_;
		SimpleAnimation anim2_;
	};

	// クラゲ
	// ...
	class JellyFish1 : public Enemy
	{
	public:
		JellyFish1(EnemyType type, Objects& obj, const Vec2& pos);

		void update() override;

		void draw() const override;

		RectF collision() const override;

	private:
		double speedX_;
		int fire_;
	};

	// コウモリ
	// ...
	class Bat1 : public Enemy
	{
	public:
		Bat1(EnemyType type, Objects& obj, const Vec2& pos, const double y);

		void update() override;

		void draw() const override;

		RectF collision() const override;

	private:
		Vec2 posBase_;
		double y_;
		Vec2 vel_;
		Timer timerSpawnBat2_;
	};

	// コウモリ
	// ...
	class Bat2 : public Enemy
	{
	public:
		Bat2(EnemyType type, Objects& obj, const Vec2& pos, const Vec2& vel);

		void update() override;

		void draw() const override;

		RectF collision() const override;

	private:
		Vec2 posBase_;
		Vec2 vel_;
	};

	// 敵弾
	// 直進する
	class Bullet1 : public Enemy
	{
	public:
		Bullet1(EnemyType type, Objects& obj, const Vec2& pos, const Vec2& vel);

		void update() override;

		void draw() const override;

		RectF collision() const override;

		void explode() override;

	private:
		Vec2 vel_;
	};

	class HandE : public Enemy
	{
	public:
		HandE(EnemyType type, Objects& obj, const Vec2& pos);

		void update() override;

		void draw() const override;

		RectF collision() const override;

		void explode() override;

	private:
		SimpleAnimation anim1_;
		SimpleAnimation anim2_;
		Vec2 vel_;
		Timer timerSpawnEnemy_;
		Timer timerFire_;
		int fireCount_;
	};


	template <class T, EnemyType E, class ... Args>
	EnemyPtr MakeEnemy(Args&& ... args)
	{
		return std::make_unique<T>(E, std::forward<Args>(args)...);
	}
}
