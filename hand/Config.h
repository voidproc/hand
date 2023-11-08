#pragma once

namespace hand
{
	struct Config
	{
		static inline constexpr StringView ConfigFilePath = U"config.ini"_sv;

		// ウィンドウのサイズ（拡大倍率）
		int windowScale;

		// スキャンラインなどのエフェクトを使う
		bool useEffect;

		// SEの音量 (0-100)
		int seVolume;

		// BGMの音量 (0-100)
		int bgmVolume;

		// ゲームコントローラの ID
		int controllerId;

		uint32 hiscore;

		Config();

		void load(FilePathView iniPath);

		void save(FilePathView iniPath);
	};
}
