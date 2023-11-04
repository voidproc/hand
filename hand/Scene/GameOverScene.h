#pragma once

namespace hand
{
	class GameOverScene : public App::Scene
	{
	public:
		GameOverScene(const InitData& init);

		void update() override;

		void draw() const override;

	private:
		Stopwatch time_;
	};
}
