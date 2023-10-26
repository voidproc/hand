#pragma once

namespace hand
{
	struct Objects;

	// メインシーンのステージイベントを実行する
	class EventList
	{
	public:
		EventList(Objects& obj);

		void load(FilePathView eventCsvPath);

		bool update(double t);

	private:
		void doEvent_();

		Objects& obj_;
		CSV eventCsv_;
		int currentRow_;
		double nextTime_;
	};
}
