#pragma once

namespace hand
{
	enum class Area : uint8
	{
		Area1 = 0,
		Area2 = 1,
		Area3 = 2,
		Area3Boss = 3,
		Clear0 = 4,
		Clear1 = 5,
	};

	String AreaToString(Area area);
}
