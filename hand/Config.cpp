#include "Config.h"

namespace hand
{
	Config::Config()
		:
		windowScale{ 3 },
		useEffect{ true },
		hiscore{ 0 },
		seVolume{ 90 },
		bgmVolume{ 70 },
		controllerId{ 0 }
	{
	}

	void Config::load(FilePathView iniPath)
	{
		INI ini{ iniPath };
		if (not ini) return;

		windowScale = ini.getOr<int>(U"WindowScale", 3);
		useEffect = ini.getOr<bool>(U"UseEffect", true);
		seVolume = ini.getOr<int>(U"SeVolume", 90);
		bgmVolume = ini.getOr<int>(U"BgmVolume", 70);
		controllerId = ini.getOr<int>(U"ControllerID", 0);
		hiscore = ini.getOr<uint32>(U"Score", 0);
	}

	void Config::save(FilePathView iniPath)
	{
		INI ini;

		ini[U"WindowScale"] = windowScale;
		ini[U"UseEffect"] = useEffect;
		ini[U"SeVolume"] = seVolume;
		ini[U"BgmVolume"] = bgmVolume;
		ini[U"ControllerID"] = controllerId;
		ini[U"Score"] = hiscore;

		ini.save(iniPath);
	}
}
