#pragma once

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
		void drawStatusBG_() const;
		void drawKarma_() const;
		void drawScore_() const;
		void drawMsg_() const;

		// 画面を振動させる
		void shake_();

		// スコアレート（補正後）
		double scoreRate_() const;

		// スコア加算
		void addScore_(double score);

		// スコアレートゲージ（秒）増加
		void addScoreRateGauge_(const Duration& gaugeAdd);

		// ステージイベントの実行
		void doEvent_(const Array<String>& e);

		AudioAsset currentBgm_();

		// 画面内のオブジェクト
		Objects obj_;
		bool handExistsPrevFrame_;

		// シーンの経過時間
		Stopwatch time_;

		// [DEBUG] 敵の生成
		Timer timerSpawnEnemy_;

		// プレイヤーが死んでいるか
		Stopwatch timePlayerDead_;

		// 画面振動用
		Timer timerShake_;
		Timer timerShakeLong_;

		// スコアレート関係
		// 敵の撃破時やアイテム取得時に加算されるスコアにかかる倍率（1.0～8.0）
		// Handを維持するとレートが増加する
		// Handがない状態ではレートが減少する
		double scoreRateRaw_;
		Stopwatch timeIncrScoreRate_;
		Timer timerScoreRateGaugeDecr_;

		// ステージイベント
		EventList eventList_;

		// BG用
		Stopwatch timeBgDarkOverlayAlpha_;
		Stopwatch timeBgRain_;
		Stopwatch timeNightSky_;
		Timer timerStar_;

		// ステージタイトル表示用
		Stopwatch timeStageTitle_;
		int stage_;

		// メッセージ
		Timer timerMsg_;
		std::pair<int, int> msgRange_;
		int currentMsg_;

		// シーン終了
		Stopwatch timeEndScene_;

		// BGM
		int bgm_;

		// SE
		Timer timerSe_;
	};
}
