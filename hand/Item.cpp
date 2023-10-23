#include "Item.h"
#include "SceneSize.h"
#include "SpriteSheet.h"

namespace hand
{
	Item::Item(ItemType type, Effect& effect, const Vec2& pos)
		:
		type_{ type },
		effect_{ effect },
		pos_{ pos },
		life_{ 1 },
		time_{ StartImmediately::Yes }
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
}
