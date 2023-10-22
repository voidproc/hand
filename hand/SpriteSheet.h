#pragma once

namespace hand
{
	namespace SpriteSheet
	{
		RectF DrawAt(const Texture& texture, int frames, const Vec2& pos, const Color& color = Palette::White, const Duration& animSpeed = 0.5s, double t = Scene::Time());
	}
}
