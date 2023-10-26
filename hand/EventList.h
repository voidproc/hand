#pragma once

#include "Enemy.h"

namespace hand
{
	// メインシーンのステージイベントを実行する
	class EventList
	{
	public:
		EventList(Effect& effect, Array<EnemyPtr>& enemies);

		void load(FilePathView eventCsvPath);

		bool update(double t);

	private:
		void doEvent_();

		Effect& effect_;
		Array<EnemyPtr>& enemies_;
		CSV eventCsv_;
		int currentRow_;
		double nextTime_;
	};
}
