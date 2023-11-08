#include "ScoreText.h"
#include "Theme.h"

void hand::DrawScoreText(const Vec2& pos, uint32 score)
{
	const String scoreText = U"{:08d}"_fmt(score);
	Vec2 penPos = pos;
	bool grayed = true;
	for (auto [index, glyph] : Indexed(FontAsset(U"H68").getGlyphs(scoreText)))
	{
		if (glyph.codePoint != U'0' || index == scoreText.size() - 1) grayed = false;

		glyph.texture.draw(penPos + glyph.getOffset() + Vec2{ 1, 1 }, ColorF{ Theme::Lighter, grayed ? 0.0 : 1.0 });
		glyph.texture.draw(penPos + glyph.getOffset(), grayed ? Theme::Darker : Theme::Black);

		penPos.x += glyph.xAdvance;
	}
}

void hand::DrawScoreTextAt(const Vec2& pos, uint32 score)
{
	DrawScoreText(pos - Vec2{ 6 * 8, 8 } * 0.5, score);
}
