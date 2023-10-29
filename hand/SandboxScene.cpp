#include "SandboxScene.h"
#include "SceneSize.h"
#include "Theme.h"

namespace hand
{
	SandboxScene::SandboxScene(const InitData& init)
		:
		IScene{ init },
		time_{ StartImmediately::Yes, GlobalClock::Get() }
	{
	}

	void SandboxScene::update()
	{
	}

	void SandboxScene::draw() const
	{
		SceneRect.draw(Theme::White);
	}
}
