#include "TextVibration.h"

Vec2 hand::GetTextVibrationOnSelectionChange(double timeSec)
{
	const double vibX = 2.0 * (1.0 - Clamp(timeSec / 0.18, 0.0, 1.0)) * Periodic::Sine1_1(0.06s);
	return Vec2{ vibX, 0 };
}
