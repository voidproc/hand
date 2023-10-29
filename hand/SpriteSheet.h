#pragma once

namespace hand
{
	namespace SpriteSheet
	{
		TextureRegion GetFrame(const Texture& texture, int frames, const Duration& animSpeed = 0.5s, double t = Scene::Time());

		RectF DrawAt(const Texture& texture, int frames, const Vec2& pos, const Color& color = Palette::White, const Duration& animSpeed = 0.5s, double t = Scene::Time());
	}
}
