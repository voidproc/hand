#include "Enemy.h"
#include "SceneSize.h"
#include "SpriteSheet.h"

namespace hand
{
	Enemy::Enemy(const Vec2& pos)
		:
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
		}
	}

	Bird1::Bird1(const Vec2& pos)
		:
		Enemy{ pos }
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

	Bird2::Bird2(const Vec2& pos)
		:
		Enemy{ pos }
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
