#pragma once

namespace hand
{
	namespace Theme
	{
		constexpr std::array<Color, 4> Colors = {
			Color{ U"#606911" },
			Color{ U"#a1a72a" },
			Color{ U"#cbd558" },
			Color{ U"#e8edba" },
		};

		constexpr Color Black = Colors[0];
		constexpr Color Darker = Colors[1];
		constexpr Color Lighter = Colors[2];
		constexpr Color White = Colors[3];
	}
}
