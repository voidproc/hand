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

		Config();

		void load(FilePathView iniPath);

		void save(FilePathView iniPath);
	};
}
