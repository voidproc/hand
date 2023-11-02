#pragma once

#include "Player.h"
#include "Hand.h"
#include "Enemy.h"
#include "Item.h"

namespace hand
{
	class InputDevice;

	// メインシーン画面内のオブジェクト
	struct Objects
	{
		Effect effect;
		Effect bgEffect;
		Array<HandPtr> hands;
		Array<EnemyPtr> enemies;
		Array<ItemPtr> items;
		Player player;

		Objects(InputDevice& input)
			:
			effect{},
			bgEffect{},
			hands{},
			enemies{},
			items{},
			player{ input, hands }
		{

		}
	};
}
