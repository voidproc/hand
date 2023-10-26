#pragma once

#include "Player.h"
#include "Hand.h"
#include "Enemy.h"
#include "Item.h"

namespace hand
{
	// メインシーン画面内のオブジェクト
	struct Objects
	{
		Effect effect;
		Array<HandPtr> hands;
		Array<EnemyPtr> enemies;
		Array<ItemPtr> items;
		Player player{ hands };
	};
}
