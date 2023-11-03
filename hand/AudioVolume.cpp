#include "AudioVolume.h"

void hand::ApplyAudioVolume(int se, int bgm)
{
	GlobalAudio::BusSetVolume(MixBus0, se * 0.01);
	GlobalAudio::BusSetVolume(MixBus1, bgm * 0.01);
}
