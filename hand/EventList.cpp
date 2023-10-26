#include "EventList.h"
#include "Objects.h"
#include "SceneSize.h"

namespace hand
{
	bool IsComment(const String& text)
	{
		return text.trimmed().isEmpty() || text.starts_with(U'#');
	}

	EventList::EventList(Objects& obj)
		:
		obj_{ obj },
		eventCsv_{},
		currentRow_{ 0 },
		nextTime_{ 0 }
	{
	}

	void EventList::load(FilePathView eventCsvPath)
	{
		if (not eventCsv_.load(eventCsvPath)) return;

		currentRow_ = 0;
	}

	bool EventList::update(double t)
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
		if (nextTime_ + time < t)
		{
			nextTime_ += time;
			doEvent_();
			++currentRow_;
			return true;
		}

		// いま実行するイベントがない
		return false;
	}

	void EventList::doEvent_()
	{
		const String textType = eventCsv_.get(currentRow_, 1).lowercased();
		const String textX = eventCsv_.get(currentRow_, 2).lowercased();
		const String textY = eventCsv_.get(currentRow_, 3).lowercased();

		Vec2 pos{};

		if (textX == U"random")
		{
			pos.x = Random(16.0, SceneWidth - 16.0);
		}
		else if (textX == U"right")
		{
			pos.x = SceneWidth + 16;
		}
		else if (textX == U"left")
		{
			pos.x = -16;
		}
		else
		{
			pos.x = eventCsv_.get<double>(currentRow_, 2);
		}

		if (textY == U"random")
		{
			pos.y = Random(32.0, SceneHeight - 16.0);
		}
		else if (textY == U"bottom")
		{
			pos.y = SceneHeight + 16;
		}
		else if (textY == U"top")
		{
			pos.y = -16;
		}
		else
		{
			pos.y = eventCsv_.get<double>(currentRow_, 3);
		}

		if (textType == U"bird1")
		{
			obj_.enemies.emplace_back(MakeEnemy<Bird1, EnemyType::Bird1>(obj_, pos));
		}
		else if (textType == U"bird2")
		{
			obj_.enemies.emplace_back(MakeEnemy<Bird2, EnemyType::Bird2>(obj_, pos));
		}
	}
}
