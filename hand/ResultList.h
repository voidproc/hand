#pragma once

#include "Area.h"
#include "Difficulty.h"

namespace hand
{
	struct GameResult
	{
		uint32 score;
		Area area;
	};

	class ResultList
	{
	public:
		static inline constexpr StringView ResultDataPath = U"score.dat"_sv;

		ResultList();

		void load(FilePathView dataPath);

		void save(FilePathView dataPath);

		void add(Difficulty difficulty, const GameResult& entry);

	private:
		void sort_();

		Array<Array<GameResult>> result_;
	};
}
