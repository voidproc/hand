#include "Config.h"

namespace hand
{
	Config::Config()
		:
		windowScale{ 3 },
		useEffect{ true },
		hiscore{ 0 }
	{
	}

	void Config::load(FilePathView iniPath)
	{
		INI ini{ iniPath };
		if (not ini) return;

		windowScale = ini.getOr<int>(U"WindowScale", 3);
		useEffect = ini.getOr<bool>(U"UseEffect", true);
		hiscore = ini.getOr<int>(U"Score", 0);
	}

	void Config::save(FilePathView iniPath)
	{
		INI ini;

		ini[U"WindowScale"] = windowScale;
		ini[U"UseEffect"] = useEffect;
		ini[U"Score"] = hiscore;

		ini.save(iniPath);
	}
}
