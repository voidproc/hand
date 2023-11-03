#include "Hiscore.h"

void hand::SaveHiscore(GameData& data)
{
	if (data.config.hiscore < data.score)
	{
		data.config.hiscore = data.score;
		data.config.save(Config::ConfigFilePath);
	}
}
