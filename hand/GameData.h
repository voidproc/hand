#pragma once

#include "Config.h"
#include "InputDevice.h"

namespace hand
{
	struct GameData
	{
		Config config;

		InputDevice input;

		// 現在のステージ
		int currentStage = 0;

		// スコア
		int score = 0;
	};
}
