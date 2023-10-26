#pragma once

//#include "Player.h"
//#include "Hand.h"
//#include "Enemy.h"
//#include "Item.h"
#include "Objects.h"
#include "EventList.h"

namespace hand
{
	class MainScene : public App::Scene
	{
	public:
		MainScene(const InitData& init);

		void update() override;

		void draw() const override;

	private:
		void updateScoreRate_();
		void checkCollision_();

		void drawBG_() const;
		void drawStageTitle_() const;
		void drawKarma_() const;
		void drawScore_() const;

		// 画面を振動させる
		void shake_();

		// スコアレート（補正後）
		double scoreRate_() const;

		// スコア加算
		void addScore_(double score);

		// 画面内のオブジェクト
		Objects obj_;
		//Effect effect_;
		//Player player_;
		//Array<HandPtr> hands_;
		//Array<EnemyPtr> enemies_;
		//Array<ItemPtr> items_;

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
		double scoreRateRaw_;
		Stopwatch timeIncrScoreRate_;
		Timer timerDecrScoreRate_;

		EventList eventList_;
	};
}
