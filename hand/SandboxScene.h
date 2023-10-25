#pragma once

namespace hand
{
	class SandboxScene : public App::Scene
	{
	public:
		SandboxScene(const InitData& init);

		void update() override;

		void draw() const override;

	private:
		Stopwatch time_;
	};
}
