﻿#include "Enemy.h"
#include "Objects.h"
#include "SceneSize.h"
#include "SpriteSheet.h"
#include "Theme.h"

namespace hand
{
	bool IsEnemy(EnemyType type)
	{
		switch (type)
		{
		case EnemyType::Bird1:
		case EnemyType::Bird2:
		case EnemyType::Bird3:
			return true;
		}

		return false;
	}

	bool IsBullet(EnemyType type)
	{
		switch (type)
		{
		case EnemyType::Bullet1:
		case EnemyType::Bullet2:
		case EnemyType::Bullet3:
			return true;
		}

		return false;
	}

	int EnemyScore(EnemyType type)
	{
		switch (type)
		{
		case EnemyType::Bird1:return 200;
		case EnemyType::Bird2:return 200;
		case EnemyType::Bird3:return 200;
		case EnemyType::Bullet1: return 50;
		case EnemyType::Bullet2: return 50;
		case EnemyType::Bullet3: return 50;
		}

		return 0;
	}


	namespace
	{
		// 爆発エフェクト
		// 爆発の破片
		struct ExplodeFragmentEffect : IEffect
		{
			ExplodeFragmentEffect(const Vec2& pos)
				: pos_{ pos }, vel_{ Circular{ Random(0.2, 3.0), Random(Math::TwoPi) } }, lifetime_{ Random(0.2, 0.45) }, prevT_{ 0 }
			{
				// 中心からちょっと進んだ位置からスタート
				pos_ += vel_ * 60.0 * Scene::DeltaTime() * 4;
			}

			bool update(double t) override
			{
				const double deltaTime = t - prevT_;
				prevT_ = t;

				vel_.y += 4.0 * deltaTime;

				pos_ += vel_ * 60.0 * deltaTime;

				const double alpha = Periodic::Square0_1(0.07s, t);
				const double size = EaseOutCubic(1.0 - t / lifetime_) * 4.0;

				RectF{ Arg::center = pos_, size }
					.rotated(Random(Math::TwoPi))
					.draw(ColorF{ Theme::Darker, alpha });

				return t < lifetime_;
			}

			Vec2 pos_;
			Vec2 vel_;
			double lifetime_;
			double prevT_;
		};

		// 爆発エフェクト
		// 爆発の中心
		struct ExplodeEffect : IEffect
		{
			ExplodeEffect(const Vec2& pos, double scale = 1.0)
				: pos_{ pos }, scale_{ scale }
			{
			}

			bool update(double t) override
			{
				constexpr double Lifetime = 0.24;
				const double alpha = Periodic::Pulse0_1(0.08s, 0.75 - 0.50 * t / Lifetime, t);
				const double size = EaseOutCubic(t / Lifetime) * 14.0 * scale_;

				Circle{ pos_, size }
				.drawFrame(8.0 - 8.0 * t / Lifetime, ColorF{ Theme::Darker, alpha });

				return t < Lifetime;
			}

			Vec2 pos_;
			double scale_;
		};

		void AddExplodeEffect(Effect& effect, const Vec2& pos)
		{
			effect.add<ExplodeEffect>(pos);

			for (int i : step(Random(6, 10)))
			{
				effect.add<ExplodeFragmentEffect>(pos);
			}
		}

		void AddExplodeEffectForBullet(Effect& effect, const Vec2& pos)
		{
			effect.add<ExplodeEffect>(pos, 0.5);
		}
	}

	Enemy::Enemy(EnemyType type, Objects& obj, const Vec2& pos)
		:
		type_{ type },
		obj_{ obj },
		pos_{ pos },
		life_{ 10 },
		time_{ StartImmediately::Yes, GlobalClock::Get() }
	{
	}

	EnemyType Enemy::type() const
	{
		return type_;
	}

	void Enemy::update()
	{
	}

	void Enemy::draw() const
	{
		// [DEBUG] 当たり判定
		//collision().drawFrame(1, 0, Palette::Magenta.withAlpha(128));
	}

	RectF Enemy::collision() const
	{
		return RectF{ Arg::center = pos_, 12 };
	}

	void Enemy::explode()
	{
		AddExplodeEffect(obj_.effect, pos_);
	}

	bool Enemy::isAlive() const
	{
		return (life_ > 0) && SceneRect.stretched(32).intersects(pos_);
	}

	void Enemy::damage(int damageAmount)
	{
		if (life_ > 0)
		{
			life_ -= damageAmount;

			// 爆発エフェクト
			if (life_ <= 0)
			{
				explode();
			}
		}
	}

	const Vec2& Enemy::pos() const
	{
		return pos_;
	}

	Bird1::Bird1(EnemyType type, Objects& obj, const Vec2& pos, double speedScale)
		:
		Enemy{ type, obj, pos }, speedScale_{ speedScale }
	{
	}

	void Bird1::update()
	{
		pos_.x -= 0.5 * 60 * Scene::DeltaTime() * speedScale_;
	}

	void Bird1::draw() const
	{
		SpriteSheet::DrawAt(TextureAsset(U"Bird"), 3, pos_, Palette::White, 0.4s, time_.sF());

		Enemy::draw();
	}

	RectF Bird1::collision() const
	{
		return RectF{ Arg::center = pos_.movedBy(0, 2), 12 };
	}

	Bird2::Bird2(EnemyType type, Objects& obj, const Vec2& pos, double speedScale)
		:
		Enemy{ type, obj, pos }, speedScale_{ speedScale }
	{
	}

	void Bird2::update()
	{
		pos_.x -= 0.8 * 60 * Scene::DeltaTime() * speedScale_;
		pos_.y += 16.0 * Periodic::Sine1_1(1.8s, time_.sF()) * Scene::DeltaTime();
	}

	void Bird2::draw() const
	{
		SpriteSheet::DrawAt(TextureAsset(U"Bird"), 3, pos_, Palette::White, 0.4s, time_.sF());

		Enemy::draw();
	}

	RectF Bird2::collision() const
	{
		return RectF{ Arg::center = pos_.movedBy(0, 2), 12 };
	}

	Bullet1::Bullet1(EnemyType type, Objects& obj, const Vec2& pos, const Vec2& vel)
		:
		Enemy{ type, obj, pos },
		vel_{ vel }
	{
	}

	void Bullet1::update()
	{
		pos_ += vel_ * 60.0 * Scene::DeltaTime();
	}

	void Bullet1::draw() const
	{
		SpriteSheet::DrawAt(TextureAsset(U"Bullet"), 4, pos_, Palette::White, 0.4s, time_.sF());

		Enemy::draw();
	}

	RectF Bullet1::collision() const
	{
		return RectF{ Arg::center = pos_, 6 };
	}

	void Bullet1::explode()
	{
		AddExplodeEffectForBullet(obj_.effect, pos_);
	}

	Bird3::Bird3(EnemyType type, Objects& obj, const Vec2& pos)
		: Enemy{ type, obj, pos }, anim_{ GlobalClock::Get() }, timerFire_{ 0.5s, StartImmediately::Yes }
	{
		anim_
			.set(U"SpeedX", { 0s, -60 }, { 2.2s, 0 }, EaseOutSine)
			.set(U"SpeedX", { 2.8s, 0 }, { 5.0s, 60 }, EaseInSine)
			.set(U"Fire", { 0s, 0 }, { 1.9s, 0 })
			.set(U"Fire", { 1.9s, 1 }, { 2.3s, 1 })
			.set(U"Fire", { 2.3s, 0 }, { 999s, 0 })
			.set(U"Mirrored", { 0s, 0 }, { 2.9s, 0 })
			.set(U"Mirrored", { 2.9s, 1 }, { 999s, 1 })
			.start();
	}

	void Bird3::update()
	{
		pos_.x += anim_[U"SpeedX"] * Scene::DeltaTime();
		pos_.y += 4.0 * Periodic::Sine1_1(1.8s, time_.sF()) * Scene::DeltaTime();

		if (timerFire_.reachedZero())
		{
			timerFire_.restart();

			if (anim_[U"Fire"] > 0)
			{
				obj_.enemies.emplace_back(MakeEnemy<Bullet1, EnemyType::Bullet1>(obj_, pos_, Circular{ 1.2, obj_.player.angleFrom(pos_) }));
			}
		}
	}

	void Bird3::draw() const
	{
		auto tex = SpriteSheet::GetFrame(TextureAsset(U"Bird"), 3, 0.4s, time_.sF());
		tex.mirrored(anim_[U"Mirrored"]).drawAt(pos_, Palette::White);

		Enemy::draw();
	}

	RectF Bird3::collision() const
	{
		return RectF{ Arg::center = pos_.movedBy(0, 2), 12 };
	}

}
