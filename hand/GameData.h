#pragma once

#include "Config.h"
#include "InputDevice.h"

namespace hand
{
	struct GameData
	{
		Config config;

		InputDevice input;

		// スコア
		int score = 0;
	};
}
