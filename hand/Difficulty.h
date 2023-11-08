#pragma once

namespace hand
{
	enum class Difficulty : uint8
	{
		Normal = 0,
		Easy = 1,
		Hard = 2,
	};

	String DifficultyToNameString(Difficulty d);
	String DifficultyToDescriptionString(Difficulty d);
	Difficulty DifficultyNext(Difficulty d);
	Difficulty DifficultyPrev(Difficulty d);
}
