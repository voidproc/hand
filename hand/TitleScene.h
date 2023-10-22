#pragma once

namespace hand
{
	class TitleScene : public App::Scene
	{
	public:
		TitleScene(const InitData& init);

		void update() override;

		void draw() const override;

	private:
		Stopwatch time_;
		Stopwatch timeEnter_;
	};
}
