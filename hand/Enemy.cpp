#include "Enemy.h"
#include "SceneSize.h"

namespace hand
{
	Enemy::Enemy(const Vec2& pos, EnemyType type)
		:
		pos_{ pos },
		type_{ type },
		life_{ 10 },
		time_{ StartImmediately::Yes }
	{
	}

	void Enemy::update()
	{
		if (type_ == EnemyType::Bird1)
		{
			pos_.x -= 0.5 * 60 * Scene::DeltaTime();
		}
	}

	void Enemy::draw() const
	{
		if (type_ == EnemyType::Bird1)
		{
			const int birdFrame = static_cast<int>(Clamp(3 * Periodic::Sawtooth0_1(0.4s), 0.0, 2.0));
			TextureAsset(U"Bird")(16 * birdFrame, 0, 16, 20).drawAt(pos_, Palette::White);
		}

		// [DEBUG] 当たり判定
		collision().drawFrame(1, 0, Palette::Magenta.withAlpha(128));
	}

	bool Enemy::isAlive() const
	{
		return (life_ > 0) && SceneRect.stretched(32).intersects(pos_);
	}

	RectF Enemy::collision() const
	{
		if (type_ == EnemyType::Bird1)
		{
			return RectF{ Arg::center = pos_.movedBy(0, 2), 12 };
		}

		return RectF{ Arg::center = pos_, 12 };
	}

	void Enemy::damage(int damageAmount)
	{
		if (life_ > 0)
		{
			life_ -= damageAmount;
		}
	}
}
