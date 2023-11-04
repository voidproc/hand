#include "stdafx.h"
#include "SpriteSheet.h"

namespace hand
{
	namespace SpriteSheet
	{
		TextureRegion GetFrame(const Texture& texture, int frames, int frameBegin, int frameEnd, const Duration& animSpeed, double t)
		{
			const int frameIndex = frameBegin + static_cast<int>(Clamp((frameEnd - frameBegin + 1) * Periodic::Sawtooth0_1(animSpeed.count(), t), 0.0, 1.0 * (frameEnd - frameBegin)));
			const double frameWidth = texture.width() / frames;
			return texture(frameWidth * frameIndex, 0, frameWidth, texture.height());
		}

		TextureRegion GetFrameNoLoop(const Texture& texture, int frames, int frameBegin, int frameEnd, const Duration& animSpeed, double t)
		{
			const int frameIndex = frameBegin + static_cast<int>(Clamp((frameEnd - frameBegin + 1) * t / animSpeed.count(), 0.0, 1.0 * (frameEnd - frameBegin)));
			const double frameWidth = texture.width() / frames;
			return texture(frameWidth * Clamp(frameIndex, frameBegin, frameEnd), 0, frameWidth, texture.height());
		}

		TextureRegion GetFrame(const Texture& texture, int frames, const Duration& animSpeed, double t)
		{
			return GetFrame(texture, frames, 0, frames - 1, animSpeed, t);
		}

		RectF DrawAt(const Texture& texture, int frames, const Vec2& pos, const Color& color, const Duration& animSpeed, double t)
		{
			return GetFrame(texture, frames, animSpeed, t).drawAt(pos, color);
		}
	}
}
