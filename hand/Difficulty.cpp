#include "Difficulty.h"

namespace hand
{
	String DifficultyToNameString(Difficulty d)
	{
		switch (d)
		{
		case Difficulty::Normal: return U"NORMAL";
		case Difficulty::Easy: return U"MILD";
		case Difficulty::Hard: return U"HARD";
		}

		return U"";
	}

	String DifficultyToDescriptionString(Difficulty d)
	{
		switch (d)
		{
		case Difficulty::Normal: return U"ふだんからシューティングゲームをたしなまれているのですか？　ではどうぞお選びください！　あなたにはすこし簡単かもしれませんが・・・";
		case Difficulty::Easy: return U"ほんのりあまくちでマイルドなむずかしさ　そんなにかまえず、よっていってくださいな　きっと楽しくあそべますから、ぜひぜひ";
		case Difficulty::Hard: return U"あなたはもしや　絶めつのキキにひんしている　「しゅーたあ」とよばれる民族のうまれでは？！　でしたらこの特別なコースをおためしください　スパイシーな味付けになっておりますので";
		}

		return U"";
	}
}
