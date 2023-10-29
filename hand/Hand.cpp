#include "Hand.h"
#include "SceneSize.h"
#include "Theme.h"

namespace hand
{
	Hand::Hand(const Vec2& pos, HandDirection dir)
		:
		pos_{ pos },
		dir_{ dir },
		ySpeed_{ 0 },
		time_{ StartImmediately::Yes, GlobalClock::Get() },
		killed_{ false }
	{
		if (dir == HandDirection::Up)
		{
			pos_.y -= 16.0;
			ySpeed_ = -50.0;
		}
		else if (dir == HandDirection::Down)
		{
			pos_.y += 16.0;
			ySpeed_ = 50.0;
		}
	}

	void Hand::update()
	{
		pos_.x += 100.0 * (1.0 - EaseOutSine(Clamp(time_.sF(), 0.0, 0.3) / 0.3)) * Scene::DeltaTime();
		pos_.y += ySpeed_ * Scene::DeltaTime();

		ySpeed_ = Max(Abs(ySpeed_) - 50.0 * 2 * Scene::DeltaTime(), 0.0) * Sign(ySpeed_);
	}

	void Hand::draw() const
	{
		const double alpha = (time_ > 0.75s) ? 0.5 + 0.5 * Periodic::Square0_1(0.08s, time_.sF()) : 1.0;
		const ColorF color{ Palette::White.lerp(Theme::Lighter, Periodic::Square0_1(0.13s, time_.sF())), alpha };

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
