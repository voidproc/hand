﻿#pragma once

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
		// 画面を振動させる
		void shake_();

		Effect effect_;

		// 画面内のオブジェクト
		Player player_;
		Array<HandPtr> hands_;
		Array<EnemyPtr> enemies_;
		Array<ItemPtr> items_;

		// シーンの経過時間
		Stopwatch time_;

		// [DEBUG] 敵の生成
		Timer timerSpawnEnemy_;

		// プレイヤーが死んでいるか
		Stopwatch timePlayerDead_;

		// 画面振動用
		Timer timerShake_;

		// スコアレート関係
		// 敵の撃破時やアイテム取得時に加算されるスコアにかかる倍率（1.0～8.0）
		// Handを維持するとレートが増加する
		// Handがない状態ではレートが減少する
		double scoreRate_;
		Stopwatch timeIncrScoreRate_;
		Timer timerDecrScoreRate_;
	};
}
