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
}
