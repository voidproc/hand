#pragma once

#include "Player.h"
#include "Hand.h"

namespace hand
{
	class MainScene : public App::Scene
	{
	public:
		MainScene(const InitData& init);

		void update() override;

		void draw() const override;

	private:
		Player player_;
		Array<HandPtr> hands_;

		Stopwatch time_;
	};
}
