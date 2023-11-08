#pragma once

#include "Config.h"
#include "ResultList.h"
#include "InputDevice.h"
#include "Difficulty.h"
#include "Area.h"

namespace hand
{
	struct GameData
	{
		Config config;

		ResultList result;

		InputDevice input{ config };

		// 難易度
		Difficulty difficulty = Difficulty::Normal;

		// スコア
		uint32 score = 0;

		// 到達エリア
		Area area = Area::Area1;

		bool endingType = 0;
	};
}
