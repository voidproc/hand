#include "stdafx.h"
#include "SpriteSheet.h"

namespace hand
{
	namespace SpriteSheet
	{
		RectF DrawAt(const Texture& texture, int frames, const Vec2& pos, const Color& color, const Duration& animSpeed, double t)
		{
			const int frameIndex = static_cast<int>(Clamp(frames * Periodic::Sawtooth0_1(animSpeed.count(), t), 0.0, 1.0 * frames));
			const double frameWidth = texture.width() / frames;
			const auto region = texture(frameWidth * frameIndex, 0, frameWidth, texture.height()).drawAt(pos, color);
			return region;
		}
	}
}
