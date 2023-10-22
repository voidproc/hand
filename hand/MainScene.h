#pragma once

#include "Player.h"
#include "Hand.h"
#include "Enemy.h"

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
		Array<EnemyPtr> enemies_;
		Stopwatch time_;

		Timer timerSpawnEnemy_;
	};
}
