#include "Enemy.h"
#include "SceneSize.h"
#include "SpriteSheet.h"
#include "Theme.h"

namespace hand
{
	// 爆発エフェクト
	// 爆発の破片
	struct ExplodeFragmentEffect : IEffect
	{
		ExplodeFragmentEffect(const Vec2& pos)
			: pos_{ pos }, vel_{ Circular{ Random(0.2, 3.0), Random(Math::TwoPi) } }, lifetime_{ Random(0.2, 0.45) }
		{
			// 中心からちょっと進んだ位置からスタート
			pos_ += vel_ * 60.0 * Scene::DeltaTime() * 4;
		}

		bool update(double t) override
		{
			vel_.y += 4.0 * Scene::DeltaTime();

			pos_.x += vel_.x * 60.0 * Scene::DeltaTime();
			pos_.y += vel_.y * 60.0 * Scene::DeltaTime();

			const double alpha = Periodic::Square0_1(0.07s);
			const double size = EaseOutCubic(1.0 - t / lifetime_) * 4.0;

			RectF{ Arg::center = pos_, size }
				.rotated(Random(Math::TwoPi))
				.draw(ColorF{Theme::Darker, alpha});

			return t < lifetime_;
		}

		Vec2 pos_;
		Vec2 vel_;
		double lifetime_;
	};

	// 爆発エフェクト
	// 爆発の中心
	struct ExplodeEffect : IEffect
	{
		ExplodeEffect(const Vec2& pos)
			: pos_{ pos }
		{
		}

		bool update(double t) override
		{
			constexpr double Lifetime = 0.24;
			const double alpha = Periodic::Pulse0_1(0.08s, 0.75 - 0.50 * t / Lifetime);
			const double size = EaseOutCubic(t / Lifetime) * 14.0;

			Circle{ pos_, size }
				//.draw(ColorF{ Theme::Lighter, alpha })
				.drawFrame(8.0 - 8.0 * t / Lifetime, ColorF{ Theme::Darker, alpha });

			return t < Lifetime;
		}

		Vec2 pos_;
	};

	void AddExplodeEffect(Effect& effect, const Vec2& pos)
	{
		effect.add<ExplodeEffect>(pos);

		for (int i : step(Random(6, 10)))
		{
			effect.add<ExplodeFragmentEffect>(pos);
		}
	}


	Enemy::Enemy(Effect& effect, const Vec2& pos)
		:
		effect_{ effect },
		pos_{ pos },
		life_{ 10 },
		time_{ StartImmediately::Yes }
	{
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
				AddExplodeEffect(effect_, pos_);
			}
		}
	}

	Bird1::Bird1(Effect& effect, const Vec2& pos)
		:
		Enemy{ effect, pos }
	{
	}

	void Bird1::update()
	{
		pos_.x -= 0.5 * 60 * Scene::DeltaTime();
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

	Bird2::Bird2(Effect& effect, const Vec2& pos)
		:
		Enemy{ effect, pos }
	{
	}

	void Bird2::update()
	{
		pos_.x -= 0.8 * 60 * Scene::DeltaTime();
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

}
