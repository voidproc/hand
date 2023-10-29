#include "Item.h"
#include "SceneSize.h"
#include "SpriteSheet.h"
#include "Theme.h"

namespace hand
{
	int ItemScore(ItemType type)
	{
		switch (type)
		{
		case ItemType::Money: return 100;
		}

		return 0;
	}

	namespace
	{
		// コイン取得エフェクト
		struct GetMoneyEffect : IEffect
		{
			GetMoneyEffect(const Vec2& pos)
				: pos_{ pos }
			{
			}

			bool update(double t) override
			{
				constexpr double Lifetime = 0.15;
				const double alpha = 0.9 - 0.5 * t / 0.15;
				const double size = 2.0 + EaseOutCubic(t / Lifetime) * 6.0;

				Circle{ pos_, size }
				.drawFrame(8.0 - 8.0 * t / Lifetime, Theme::Lighter.lerp(Theme::Darker, Periodic::Square0_1(0.01s, t)));

				return t < Lifetime;
			}

			Vec2 pos_;
		};
	}

	Item::Item(ItemType type, Effect& effect, const Vec2& pos)
		:
		type_{ type },
		effect_{ effect },
		pos_{ pos },
		life_{ 1 },
		time_{ StartImmediately::Yes, GlobalClock::Get() }
	{
	}

	ItemType Item::type() const
	{
		return type_;
	}

	void Item::update()
	{
	}

	void Item::draw() const
	{
		// [DEBUG] 当たり判定
		//collision().drawFrame(1, 0, Palette::Magenta.withAlpha(128));
	}

	RectF Item::collision() const
	{
		return RectF{ Arg::center = pos_, 12 };
	}

	bool Item::isAlive() const
	{
		return (life_ > 0) && SceneRect.stretched(32).intersects(pos_);
	}

	void Item::kill()
	{
		life_ = 0;

		effect_.add<GetMoneyEffect>(pos_);
	}

	bool Item::enableCollision() const
	{
		return true;
	}

	const Vec2& Item::pos() const
	{
		return pos_;
	}

	ItemMoney::ItemMoney(ItemType type, Effect& effect, const Vec2& pos)
		: Item{ type, effect, pos }, vel_{ Circular{ Random(1.0, 2.4), Random(-20_deg, 20_deg) } }, animSpeed_{ Random(0.2, 0.8) }
	{
	}

	void ItemMoney::update()
	{
		vel_.y += 3.5 * Scene::DeltaTime();

		pos_ += vel_ * 60.0 * Scene::DeltaTime();
	}

	void ItemMoney::draw() const
	{
		SpriteSheet::DrawAt(TextureAsset(U"Money"), 6, pos_, Palette::White, SecondsF{ animSpeed_ }, time_.sF());

		Item::draw();
	}

	RectF ItemMoney::collision() const
	{
		return RectF{ Arg::center = pos_, SizeF{ 8, 12 } };
	}

	bool ItemMoney::enableCollision() const
	{
		// コインが飛び出すところを見せるために最初は衝突判定しない
		return time_ > 0.5s;
	}
}
