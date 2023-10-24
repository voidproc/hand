#pragma once

#include "Player.h"
#include "Hand.h"
#include "Enemy.h"
#include "Item.h"

namespace hand
{
	class MainScene : public App::Scene
	{
	public:
		MainScene(const InitData& init);

		void update() override;

		void draw() const override;

	private:
		Effect effect_;
		Player player_;
		Array<HandPtr> hands_;
		Array<EnemyPtr> enemies_;
		Array<ItemPtr> items_;
		Stopwatch time_;
		Timer timerSpawnEnemy_;
		Stopwatch timePlayerDead_;
	};
}
