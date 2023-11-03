#pragma once

#include "Config.h"
#include "InputDevice.h"

namespace hand
{
	struct GameData
	{
		Config config;

		InputDevice input{ config };

		// スコア
		int score = 0;

		bool endingType = 0;
	};
}
