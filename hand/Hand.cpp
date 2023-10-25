#include "Hand.h"
#include "SceneSize.h"
#include "Theme.h"

namespace hand
{
	Hand::Hand(const Vec2& pos)
		: pos_{ pos }, time_{ StartImmediately::Yes }, killed_{ false }
	{
	}

	void Hand::update()
	{
		pos_.x += 100.0 * (1.0 - EaseOutSine(Clamp(time_.sF(), 0.0, 0.3) / 0.3)) * Scene::DeltaTime();
	}

	void Hand::draw() const
	{
		const double alpha = (time_ > 0.75s) ? Periodic::Square0_1(0.08s) : 1.0;
		const ColorF color{ Palette::White.lerp(Theme::Lighter, Periodic::Square0_1(0.13s)), alpha };

		TextureAsset(U"Hand").drawAt(pos_, color);

		// [DEBUG] 当たり判定
		//collision().drawFrame(1, 0, Palette::Magenta.withAlpha(128));
	}

	bool Hand::isAlive() const
	{
		if (killed_) return false;

		if (time_ < 0.3s) return true;

		if (not KeySpace.pressed()) return false;

		return true;
	}

	RectF Hand::collision() const
	{
		return RectF{ Arg::center = pos_.movedBy(0, -2), SizeF{16, 40}};
	}

	void Hand::kill()
	{
		killed_ = true;
	}
}
