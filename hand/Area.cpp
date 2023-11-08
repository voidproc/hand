#include "Area.h"

namespace hand
{
	String AreaToString(Area area)
	{
		switch (area)
		{
		case Area::Area1: return U"AREA1";
		case Area::Area2: return U"AREA2";
		case Area::Area3: return U"AREA3";
		case Area::Area3Boss: return U"BOSS";
		case Area::Clear0: return U"CLEAR";
		case Area::Clear1: return U"CLEAR";
		}

		return U"";
	}
}
