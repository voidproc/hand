#include "AssetPreload.h"

namespace hand
{
	void PreloadAssets()
	{
		for (auto&& [name, info] : FontAsset::Enumerate())
		{
			FontAsset::LoadAsync(name, U"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz,./;:[](){}@-=+*<>?!#$%&_");
		}

		for (auto&& [name, info] : TextureAsset::Enumerate())
		{
			TextureAsset::LoadAsync(name);
		}

		for (auto&& [name, info] : AudioAsset::Enumerate())
		{
			AudioAsset::LoadAsync(name);
		}
	}

	bool IsCompletedPreloadingAssets()
	{
		for (auto&& [name, info] : FontAsset::Enumerate())
		{
			if (not FontAsset::IsReady(name)) return false;
		}

		for (auto&& [name, info] : TextureAsset::Enumerate())
		{
			if (not TextureAsset::IsReady(name)) return false;
		}

		for (auto&& [name, info] : AudioAsset::Enumerate())
		{
			if (not AudioAsset::IsReady(name)) return false;
		}

		return true;
	}
}
