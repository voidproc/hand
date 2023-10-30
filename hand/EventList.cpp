#include "EventList.h"
#include "Objects.h"
#include "SceneSize.h"

namespace hand
{
	namespace
	{
		bool IsComment(const String& text)
		{
			return text.trimmed().isEmpty() || text.starts_with(U'#');
		}
	}

	EventList::EventList(Objects& obj)
		:
		obj_{ obj },
		eventCsv_{},
		currentRow_{ 0 },
		elapsedTime_{ 0 }
	{
	}

	void EventList::load(FilePathView eventCsvPath)
	{
		if (not eventCsv_.load(eventCsvPath)) return;

		currentRow_ = 0;
	}

	bool EventList::hasActiveEvent(double t)
	{
		// 終端に達している
		if (currentRow_ >= eventCsv_.rows()) return false;

		// コメント・空行を読み飛ばす
		{
			String cell0 = eventCsv_.get(currentRow_, 0);

			while (IsComment(cell0))
			{
				++currentRow_;
				if (currentRow_ >= eventCsv_.rows()) return false;
				cell0 = eventCsv_.get(currentRow_, 0);
			} 
		}

		const double time = eventCsv_.get<double>(currentRow_, 0);

		// いま実行するイベントがあるか？
		if (elapsedTime_ + time < t)
		{
			return true;
		}

		// いま実行するイベントがない
		return false;
	}

	void EventList::next()
	{
		const double time = eventCsv_.get<double>(currentRow_, 0);
		elapsedTime_ += time;

		++currentRow_;
	}

	const Array<String>& EventList::getRow() const
	{
		return eventCsv_.getRow(currentRow_);
	}

	void EventList::gotoLabel(StringView label)
	{
		int row = currentRow_;

		while (row < eventCsv_.rows())
		{
			String cell0 = eventCsv_.get(row, 0).trimmed();

			if (U":{}"_fmt(label).case_insensitive_equals(cell0))
			{
				currentRow_ = row;
				return;
			}

			++row;
		}
	}
}
