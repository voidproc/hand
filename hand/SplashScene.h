#pragma once

namespace hand
{
	class SplashScene : public App::Scene
	{
	public:
		SplashScene(const InitData& init);

		void update() override;

		void draw() const override;

	private:
		Stopwatch time_;
	};
}
