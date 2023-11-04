#pragma once

namespace hand
{
	class ConfigScene : public App::Scene
	{
	public:
		ConfigScene(const InitData& init);

		void update() override;

		void draw() const override;

	private:
		Stopwatch time_;
		int cursor_;
	};
}
