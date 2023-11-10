﻿#pragma once

namespace hand
{
	class RankingScene : public App::Scene
	{
	public:
		RankingScene(const InitData& init);

		void update() override;

		void draw() const override;

	private:
		Stopwatch time_;
		Stopwatch timeVibHeader_;
		Stopwatch timeSlide_;
		Difficulty difficulty_;
	};
}