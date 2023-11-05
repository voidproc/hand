#include "AudioPlay.h"

namespace hand
{
	constexpr int AudioVoiceCountMax = 32;

	void PlayAudioOneShot(StringView assetName)
	{
		if (GlobalAudio::GetActiveVoiceCount() < AudioVoiceCountMax)
		{
			AudioAsset(assetName).stopAllShots();
			AudioAsset(assetName).playOneShot();
		}
	}

	void StopAllBgm(const Duration& fade)
	{
		AudioAsset(U"Bgm1").stop(fade);
		AudioAsset(U"Bgm2").stop(fade);
		AudioAsset(U"Bgm3").stop(fade);
		AudioAsset(U"Ending").stop(fade);
	}
}
