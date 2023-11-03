#pragma once

namespace hand
{
	class EndingScene : public App::Scene
	{
	public:
		EndingScene(const InitData& init);

		void update() override;

		void draw() const override;

	private:
		bool isFinishedMsg_() const;

		Stopwatch time_;
		Timer timerMsg_;
		std::pair<int, int> msgRange_;
		int msg_;
	};
}
