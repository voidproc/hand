#include "EventList.h"
#include "Objects.h"
#include "SceneSize.h"

namespace hand
{
	namespace
	{
		struct GenerateEnemies : IEffect
		{
			GenerateEnemies(std::function<void()> f, double lifetime, double interval)
				:
				f_{ f },
				lifetime_{ lifetime },
				timerInterval_{ SecondsF{ interval }, StartImmediately::Yes, GlobalClock::Get() }
			{
			}

			bool update(double t) override
			{
				if (timerInterval_.reachedZero())
				{
					timerInterval_.restart();
					f_();
				}

				return t < lifetime_;
			}

			std::function<void()> f_;
			double lifetime_;
			Timer timerInterval_;
		};

		bool IsComment(const String& text)
		{
			return text.trimmed().isEmpty() || text.starts_with(U'#');
		}

		double RandomX()
		{
			return Random(16.0, SceneWidth - 16.0);
		}

		double PosRight()
		{
			return SceneWidth + 16;
		}

		double PosLeft()
		{
			return -16;
		}

		double ParseX(const String& text)
		{
			if (text == U"random")
			{
				return RandomX();
			}
			else if (text == U"right")
			{
				return PosRight();
			}
			else if (text == U"left")
			{
				return PosLeft();
			}

			return ParseFloat<double>(text);
		}

		double RandomY()
		{
			return Random(16.0, SceneHeight - 16.0);
		}

		double PosBottom()
		{
			return SceneHeight + 16;
		}

		double PosTop()
		{
			return -16;
		}

		double ParseY(const String& text)
		{
			if (text == U"random")
			{
				return RandomY();
			}
			else if (text == U"bottom")
			{
				return PosBottom();
			}
			else if (text == U"top")
			{
				return PosTop();
			}

			return ParseFloat<double>(text);
		}
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
		const String textType = eventCsv_.get(currentRow_, 1).trimmed().lowercased();
		const String textX = eventCsv_.get(currentRow_, 2).trimmed().lowercased();
		const String textY = eventCsv_.get(currentRow_, 3).trimmed().lowercased();

		Vec2 pos{ ParseX(textX), ParseY(textY) };

		if (textType == U"bird1")
		{
			const double speedScale = eventCsv_.getOr<double>(currentRow_, 4, 1.0);
			obj_.enemies.emplace_back(MakeEnemy<Bird1, EnemyType::Bird1>(obj_, pos, speedScale));
		}
		else if (textType == U"bird2")
		{
			const double speedScale = eventCsv_.getOr<double>(currentRow_, 4, 1.0);
			obj_.enemies.emplace_back(MakeEnemy<Bird2, EnemyType::Bird2>(obj_, pos, speedScale));
		}
		else if (textType == U"bird3")
		{
			obj_.enemies.emplace_back(MakeEnemy<Bird3, EnemyType::Bird3>(obj_, pos));
		}
		else if (textType == U"birdb1")
		{
			const double speedY0 = eventCsv_.getOr<double>(currentRow_, 4, 200);
			obj_.enemies.emplace_back(MakeEnemy<BirdB1, EnemyType::BirdB1>(obj_, pos, speedY0));
		}
		else if (textType == U"genbird1")
		{
			const double lifetime = eventCsv_.get<double>(currentRow_, 4);
			const double interval = eventCsv_.get<double>(currentRow_, 5);
			const double speedScale = eventCsv_.get<double>(currentRow_, 6);
			obj_.effect.add<GenerateEnemies>([&, speedScale, textX, textY]() {
				const Vec2 pos{ ParseX(textX), ParseY(textY) };
				obj_.enemies.emplace_back(MakeEnemy<Bird1, EnemyType::Bird1>(obj_, pos, speedScale));
				}, lifetime, interval);
		}
		else if (textType == U"genbird2")
		{
			const double lifetime = eventCsv_.get<double>(currentRow_, 4);
			const double interval = eventCsv_.get<double>(currentRow_, 5);
			const double speedScale = eventCsv_.get<double>(currentRow_, 6);
			obj_.effect.add<GenerateEnemies>([&, speedScale, textX, textY]() {
				const Vec2 pos{ ParseX(textX), ParseY(textY) };
				obj_.enemies.emplace_back(MakeEnemy<Bird2, EnemyType::Bird2>(obj_, pos, speedScale));
				}, lifetime, interval);
		}
	}
}
