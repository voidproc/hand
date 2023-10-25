#pragma once

#include "Config.h"

namespace hand
{
	struct GameData
	{
		Config config;

		// 現在のステージ
		int currentStage = 0;

		// スコア
		int score = 0;
	};
}
