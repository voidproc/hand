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

		bool hasActiveEvent(double t);

		void next();

		const Array<String>& getRow() const;

		void gotoLabel(StringView label);

	private:
		Objects& obj_;
		CSV eventCsv_;
		int currentRow_;
		double elapsedTime_;
	};
}
