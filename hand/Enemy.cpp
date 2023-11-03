#include "Enemy.h"
#include "Objects.h"
#include "SceneSize.h"
#include "SpriteSheet.h"
#include "Theme.h"
#include "DebugMode.h"

namespace hand
{
	bool IsEnemy(EnemyType type)
	{
		switch (type)
		{
		case EnemyType::Bird1:
		case EnemyType::Bird2:
		case EnemyType::Bird3:
		case EnemyType::Bird4:
		case EnemyType::BirdB1:
		case EnemyType::JellyFish1:
		case EnemyType::Bat1:
		case EnemyType::Bat2:
		case EnemyType::HandE:
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
		case EnemyType::Bird1: return 200;
		case EnemyType::Bird2: return 200;
		case EnemyType::Bird3: return 200;
		case EnemyType::Bird4: return 1000;
		case EnemyType::BirdB1: return 300;
		case EnemyType::JellyFish1: return 350;
		case EnemyType::Bat1: return 400;
		case EnemyType::Bat2: return 200;
		case EnemyType::HandE: return 0;
		case EnemyType::Bullet1: return 50;
		case EnemyType::Bullet2: return 50;
		case EnemyType::Bullet3: return 50;
		}

		return 0;
	}

	int EnemyCoins(EnemyType type)
	{
		switch (type)
		{
		case EnemyType::Bird1: return 4;
		case EnemyType::Bird2: return 4;
		case EnemyType::Bird3: return 4;
		case EnemyType::Bird4: return 12;
		case EnemyType::BirdB1: return 8;
		case EnemyType::JellyFish1: return 5;
		case EnemyType::Bat1: return 6;
		case EnemyType::Bat2: return 4;
		case EnemyType::HandE: return 0;
		case EnemyType::Bullet1: return 0;
		case EnemyType::Bullet2: return 0;
		case EnemyType::Bullet3: return 0;
		}

		return 4;
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

			for ([[maybe_unused]] int i : step(Random(6, 10)))
			{
				effect.add<ExplodeFragmentEffect>(pos);
			}
		}

		void AddExplodeEffectForBullet(Effect& effect, const Vec2& pos)
		{
			effect.add<ExplodeEffect>(pos, 0.5);
		}

		struct GenerateExplodeEffect : IEffect
		{
			GenerateExplodeEffect(Effect& effect, const Vec2& pos, int n)
				: effect_{ effect }, pos_{ pos }, timer_{ 0.2s, StartImmediately::Yes, GlobalClock::Get() }, n_{ n }
			{
			}

			bool update(double) override
			{
				if (n_ <= 0) return false;

				if (timer_.reachedZero())
				{
					timer_.restart();
					--n_;
					AddExplodeEffect(effect_, pos_ + Circular{ Random(4.0, 12.0), Random(Math::TwoPi) });
					AudioAsset(U"Explosion").playOneShot();
				}

				return true;
			}

			Effect& effect_;
			Vec2 pos_;
			Timer timer_;
			int n_;
		};

		struct DestroyedHandEffect : IEffect
		{
			DestroyedHandEffect(const Vec2& pos)
				: pos_{ pos }
			{
			}

			bool update(double t) override
			{
				TextureAsset(U"Hand").scaled(2).mirrored().drawAt(RandomVec2(Circle{ pos_, 4.0 }), AlphaF(0.3 + 0.5 * Periodic::Square0_1(0.07s, t)));
				return t < 2.1;
			}

			Vec2 pos_;
		};
	}

	Enemy::Enemy(EnemyType type, Objects& obj, const Vec2& pos)
		:
		type_{ type },
		obj_{ obj },
		pos_{ pos },
		life_{ 0.1 },
		time_{ StartImmediately::Yes, GlobalClock::Get() },
		timerDamaged_{ 0.2s, StartImmediately::No }
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
#ifdef DEBUG_MODE
		// [DEBUG] 当たり判定
		//collision().drawFrame(1, 0, Palette::Magenta.withAlpha(128));
#endif
	}

	RectF Enemy::collision() const
	{
		return RectF{ Arg::center = pos_, 12 };
	}

	void Enemy::explode()
	{
		AddExplodeEffect(obj_.effect, pos_);

		AudioAsset(U"Explosion").playOneShot();
	}

	bool Enemy::isAlive() const
	{
		return (life_ > 0) && SceneRect.stretched(32).intersects(pos_);
	}

	void Enemy::damage(double damageAmount)
	{
		if (life_ > 0)
		{
			life_ -= damageAmount;
			timerDamaged_.restart();

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

	Vec2 Enemy::drawPos() const
	{
		return timerDamaged_.isRunning() ? pos_.movedBy(RandomVec2(2.0)) : pos_;
	}

	Bird1::Bird1(EnemyType type, Objects& obj, const Vec2& pos, double speedScale, double accel)
		:
		Enemy{ type, obj, pos }, vel_{ 0.5 * 60, 0 }, speedScale_{ speedScale }, accel_{ accel }
	{
	}

	void Bird1::update()
	{
		pos_.x -= vel_.x * Scene::DeltaTime() * speedScale_;
		vel_.x += accel_ * Scene::DeltaTime();
	}

	void Bird1::draw() const
	{
		SpriteSheet::DrawAt(TextureAsset(U"Bird"), 3, drawPos(), Palette::White, 0.4s, time_.sF());

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
		SpriteSheet::DrawAt(TextureAsset(U"Bird"), 3, drawPos(), Palette::White, 0.4s, time_.sF());

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
		const auto tex = SpriteSheet::GetFrame(TextureAsset(U"Bullet"), 4, 0.4s, time_.sF());
		tex.scaled(Clamp(2.0 - 1.0 * time_.sF() / 0.15, 1.0, 2.0)).drawAt(drawPos(), Palette::White);

		Enemy::draw();
	}

	RectF Bullet1::collision() const
	{
		return RectF{ Arg::center = pos_, 6 };
	}

	void Bullet1::explode()
	{
		AddExplodeEffectForBullet(obj_.effect, pos_);

		//AudioAsset(U"Explosion").playOneShot();
	}

	Bird3::Bird3(EnemyType type, Objects& obj, const Vec2& pos)
		:
		Enemy{ type, obj, pos },
		anim_{ GlobalClock::Get() },
		timerFire_{ 0.5s, StartImmediately::Yes }
	{
		life_ = 4.0;

		anim_
			.set(U"SpeedX", { 0s, -60 }, { 1.6s, -20 }, EaseOutSine)
			.set(U"SpeedX", { 1.6s, -20 }, { 3.0s, -80 }, EaseInSine)
			.set(U"Fire", { 0s, 0 }, { 1.5s, 0 })
			.set(U"Fire", { 1.5s, 1 }, { 2.0s, 1 })
			.set(U"Fire", { 2.0s, 0 }, { 999s, 0 })
			//.set(U"Mirrored", { 0s, 0 }, { 2.9s, 0 })
			//.set(U"Mirrored", { 2.9s, 1 }, { 999s, 1 })
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
		tex.drawAt(drawPos(), Palette::White);

		Enemy::draw();
	}

	RectF Bird3::collision() const
	{
		return RectF{ Arg::center = pos_.movedBy(0, 2), 12 };
	}

	BirdB1::BirdB1(EnemyType type, Objects& obj, const Vec2& pos, double speedY0)
		:
		Enemy{ type, obj, pos },
		anim1_{ GlobalClock::Get() },
		anim2_{ GlobalClock::Get() }
	{
		life_ = 4.0;

		anim1_
			.set(U"SpeedY", { 0s, speedY0 }, { 1.3s, 0 }, EaseOutSine)
			.start();

		anim2_
			.set(U"SpeedX", { 0s, -20 }, { 1.2s, -180 }, EaseInSine)
			.set(U"Texture", { 0s, 0 }, { 0.01s, 0 })
			.set(U"Texture", { 0.01s, 1 }, { 999s, 1 });
	}

	void BirdB1::update()
	{
		const bool isStartedAnim2 = anim2_.posSec() > 1e-3;

		if (anim1_.posSec() > 1.0)
		{
			if (not isStartedAnim2)
			{
				anim2_.start();
			}
		}

		pos_.x += (isStartedAnim2 ? anim2_[U"SpeedX"] : 0) * Scene::DeltaTime();
		pos_.x += 10 * Scene::DeltaTime();
		pos_.y += anim1_[U"SpeedY"] * Scene::DeltaTime();
	}

	void BirdB1::draw() const
	{
		std::pair<int, int> frame = (anim2_[U"Texture"] < 1) ? std::make_pair(0, 1) : std::make_pair(2, 3);

		auto tex = SpriteSheet::GetFrame(TextureAsset(U"BirdB"), 4, frame.first, frame.second, 0.10s, time_.sF());
		tex.drawAt(drawPos(), Palette::White);

		Enemy::draw();
	}

	RectF BirdB1::collision() const
	{
		return RectF{ Arg::center = pos_, 14, 8 };
	}

	JellyFish1::JellyFish1(EnemyType type, Objects& obj, const Vec2& pos)
		:
		Enemy{ type, obj, pos },
		speedX_{ Random(-16.0, 4.0) },
		fire_{ 0 }
	{
	}

	void JellyFish1::update()
	{
		pos_.y -= 16 * Scene::DeltaTime();
		pos_.y -= 48 * Periodic::Sine1_1(1.2s, time_.sF()) * Scene::DeltaTime();
		pos_.x += speedX_ * Scene::DeltaTime();

		if (fire_ == 0 && pos_.y < 80)
		{
			for (int i : step(8))
			{
				obj_.enemies.emplace_back(MakeEnemy<Bullet1, EnemyType::Bullet1>(obj_, pos_, Circular{ 1.3, i * Math::TwoPi / 8 }));
			}
			fire_ = 1;
		}
	}

	void JellyFish1::draw() const
	{
		auto tex = SpriteSheet::GetFrame(TextureAsset(U"JellyFish"), 3, 0.50s, time_.sF());
		tex.drawAt(drawPos(), Palette::White);
	}

	RectF JellyFish1::collision() const
	{
		return RectF{ Arg::center = pos_.movedBy(0, 2), 8, 8 };
	}

	Bat1::Bat1(EnemyType type, Objects& obj, const Vec2& pos, const double y)
		:
		Enemy{ type, obj, pos },
		posBase_{ pos },
		y_{ y },
		vel_{ Circular{ 100.0, 185_deg } },
		timerSpawnBat2_{ 0.2s, StartImmediately::No, GlobalClock::Get() }
	{
		life_ = 3.0;
	}

	void Bat1::update()
	{
		posBase_ += vel_ * Scene::DeltaTime();

		if (posBase_.y > y_)
		{
			if (not timerSpawnBat2_.isRunning())
			{
				vel_ = Circular{ 30.0, obj_.player.angleFrom(pos_) };
				timerSpawnBat2_.start();
			}
		}

		if (timerSpawnBat2_.reachedZero())
		{
			timerSpawnBat2_.restart(999s);

			for (int i : step(8))
			{
				obj_.enemies.emplace_back(MakeEnemy<Bat2, EnemyType::Bat2>(obj_, pos_, Circular{ 45.0, i * Math::TwoPi / 8 }));
			}
		}

		pos_ = posBase_ + Vec2{ 0, 13 * (0.3 + 0.7 * Periodic::Sine0_1(0.8s)) * Periodic::Triangle1_1(0.24s, time_.sF()) };
	}

	void Bat1::draw() const
	{
		auto tex = SpriteSheet::GetFrame(TextureAsset(U"Bat"), 4, 0.20s, time_.sF());
		tex.drawAt(drawPos(), Palette::White);

		Enemy::draw();
	}

	RectF Bat1::collision() const
	{
		return RectF{ Arg::center = pos_.movedBy(0, 2), SizeF{ 10, 6 } };
	}

	Bat2::Bat2(EnemyType type, Objects& obj, const Vec2& pos, const Vec2& vel)
		:
		Enemy{ type, obj, pos },
		posBase_{ pos },
		vel_{ vel }
	{
	}

	void Bat2::update()
	{
		vel_.x = Max(vel_.x - 50.0 * Scene::DeltaTime(), -80.0);

		posBase_ += vel_ * Scene::DeltaTime();

		pos_ = posBase_ + Vec2{ 0, 4 * (0.3 + 0.7 * Periodic::Sine0_1(0.8s)) * Periodic::Triangle1_1(0.24s, time_.sF()) };

	}

	void Bat2::draw() const
	{
		auto tex = SpriteSheet::GetFrame(TextureAsset(U"Bat"), 4, 0.20s, time_.sF());
		tex.drawAt(drawPos(), Palette::White);

		Enemy::draw();
	}

	RectF Bat2::collision() const
	{
		return RectF{ Arg::center = pos_.movedBy(0, 2), SizeF{ 10, 6 } };
	}

	Bird4::Bird4(EnemyType type, Objects& obj, const Vec2& pos)
		:
		Enemy{ type, obj, pos },
		anim_{ GlobalClock::Get() },
		timerFire_{ 0.75s, StartImmediately::Yes }
	{
		life_ = 10.0;

		anim_
			.set(U"SpeedX", { 0s, -80 }, { 1.1s, -4 }, EaseOutSine)
			.set(U"SpeedX", { 1.8s, -4 }, { 4.0s, -50 }, EaseInSine)
			.set(U"Fire", { 0s, 0 }, { 0.85s, 0 })
			.set(U"Fire", { 0.85s, 1 }, { 2.36s, 1 })
			.set(U"Fire", { 2.36s, 0 }, { 999s, 0 })
			.start();
	}

	void Bird4::update()
	{
		pos_.x += anim_[U"SpeedX"] * Scene::DeltaTime();
		pos_.y += 6.0 * Periodic::Sine1_1(2.2s, time_.sF()) * Scene::DeltaTime();

		if (timerFire_.reachedZero())
		{
			timerFire_.restart();

			if (anim_[U"Fire"] > 0)
			{
				for (int i = -1; i <= 1; ++i)
				{
					obj_.enemies.emplace_back(MakeEnemy<Bullet1, EnemyType::Bullet1>(obj_, pos_, Circular{ 1.4, 270_deg + i * 36_deg }));
				}
			}
		}
	}

	void Bird4::draw() const
	{
		auto tex = SpriteSheet::GetFrame(TextureAsset(U"Bird"), 3, 0.45s, time_.sF());
		tex.scaled(2).drawAt(drawPos(), Palette::White);

		Enemy::draw();
	}

	RectF Bird4::collision() const
	{
		return RectF{ Arg::center = pos_.movedBy(0, 4), 20 };
	}

	HandE::HandE(EnemyType type, Objects& obj, const Vec2& pos)
		:
		Enemy{ type, obj, pos },
		anim1_{ GlobalClock::Get() },
		anim2_{ GlobalClock::Get() },
		timerSpawnEnemy_{ 0.7s, StartImmediately::No, GlobalClock::Get() },
		timerFire_{ 0.8s, StartImmediately::No, GlobalClock::Get() },
		fireCount_{ 0 }
	{
		life_ = 1.0;

		anim1_
			.set(U"SpeedX", { 0s, -40 }, { 3.0s, 0 }, EaseInOutCubic)

			.set(U"SpeedX", { 7.3s, 0 }, { 7.6s, 30 }, EaseInOutCubic)
			.set(U"SpeedX", { 7.6s, 30 }, { 7.9s, 0 }, EaseInOutCubic)

			.set(U"SpeedX", { 7.9s, 0 }, { 8.7s, -70 }, EaseInOutCubic)
			.set(U"SpeedX", { 8.7s, -70 }, { 9.5s, 0 }, EaseInOutCubic)

			.set(U"SpeedX", { 9.6s, 0 }, { 10.4s, 60 }, EaseInOutCubic)
			.set(U"SpeedX", { 10.4s, 60 }, { 11.2s, 0 }, EaseInOutCubic)

			.set(U"SpeedY", { 4.0s, 0 }, { 4.3s, -90 }, EaseInOutCubic)
			.set(U"SpeedY", { 4.3s, -90 }, { 4.6s, 0 }, EaseInOutCubic)

			.set(U"SpeedY", { 5.1s, 0 }, { 5.4s, 90 }, EaseInOutCubic)
			.set(U"SpeedY", { 5.4s, 90 }, { 5.7s, 0 }, EaseInOutCubic)

			.set(U"SpeedY", { 6.2s, 0 }, { 6.5s, 90 }, EaseInOutCubic)
			.set(U"SpeedY", { 6.5s, 90 }, { 6.8s, 0 }, EaseInOutCubic)

			.set(U"SpeedY", { 7.3s, 0 }, { 7.6s, -90 }, EaseInOutCubic)
			.set(U"SpeedY", { 7.6s, -90 }, { 7.9s, 0 }, EaseInOutCubic)

			.setLoop(4.0s, 11.3s)
			.start();

	}

	void HandE::update()
	{
		if (time_ > 33s)
		{
			pos_.x += 20 * Scene::DeltaTime();
		}

		if (timerSpawnEnemy_.reachedZero())
		{
			timerSpawnEnemy_.restart();

			const Vec2 enemyPos{ SceneWidth + 16, pos_.y + Random(-8, 8) };

			if (RandomBool())
			{
				obj_.enemies.emplace_back(MakeEnemy<Bird1, EnemyType::Bird1>(obj_, enemyPos, 2.0 + Random(-1.0, 1.0) ));
			}
			else
			{
				obj_.enemies.emplace_back(MakeEnemy<Bird2, EnemyType::Bird2>(obj_, enemyPos, 1.6 + Random(-0.7, 0.7)));
			}
		}

		if (timerFire_.reachedZero())
		{
			timerFire_.restart(SecondsF{ Random(0.3, 0.8) });

			if ((++fireCount_) % 2 == 0)
			{
				for (int i : step(12))
				{
					obj_.enemies.emplace_back(MakeEnemy<Bullet1, EnemyType::Bullet2>(obj_, pos_, Circular{ 1.3, obj_.player.angleFrom(pos_) + i * Math::TwoPi / 8 }));
				}
			}
		}

		if (time_ > 4.0s && not timerSpawnEnemy_.isRunning())
		{
			timerSpawnEnemy_.restart();
			timerFire_.restart();
		}

		pos_.x += anim1_[U"SpeedX"] * Scene::DeltaTime();
		pos_.y += anim1_[U"SpeedY"] * Scene::DeltaTime();
		pos_.y += 10.0 * Periodic::Sine1_1(2.8s, time_.sF()) * Scene::DeltaTime();
	}

	void HandE::draw() const
	{
		const double alpha = 0.7 + 0.3 * Periodic::Square0_1(0.08s, time_.sF());
		const ColorF color{ Palette::White.lerp(Theme::Lighter, Periodic::Square0_1(0.13s, time_.sF())), alpha };

		TextureAsset(U"Hand").scaled(2).mirrored().drawAt(drawPos(), color);

		Enemy::draw();
	}

	RectF HandE::collision() const
	{
		if (time_ < 3.9s) return RectF{};

		return RectF{ Arg::center = pos_.movedBy(0, -4), SizeF{ 16, 40 } * 2 };
	}

	void HandE::explode()
	{
		obj_.effect.add<GenerateExplodeEffect>(obj_.effect, pos_, 10);
		obj_.effect.add<DestroyedHandEffect>(pos_);
	}

}
