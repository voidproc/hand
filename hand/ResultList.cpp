#include "ResultList.h"

namespace hand
{
	namespace
	{
		constexpr int EntryCount = 8;
	}

	ResultList::ResultList()
	{
		result_.resize(3);

		for (int iEntry = 0; iEntry < EntryCount; ++iEntry)
		{
			result_[0].push_back(GameResult{ 2000u + 800 * iEntry, Area::Area1 });
			result_[1].push_back(GameResult{ 1000u + 600 * iEntry, Area::Area1 });
			result_[2].push_back(GameResult{ 4000u + 1200 * iEntry, Area::Area1 });
		}

		sort_();
	}

	void ResultList::load(FilePathView dataPath)
	{
		// ファイルが存在しない場合、デフォルトのスコアデータを保存する
		if (not FileSystem::Exists(dataPath))
		{
			save(dataPath);
			return;
		}

		// ファイルサイズが足りない場合、デフォルトのスコアデータを保存する
		BinaryReader reader{ dataPath };
		if (reader.size() < 3 * EntryCount * (sizeof(uint32) + sizeof(uint8)))
		{
			save(dataPath);
			return;
		}

		for (int iDiff = 0; iDiff < 3; ++iDiff)
		{
			for (int iEntry = 0; iEntry < EntryCount; ++iEntry)
			{
				reader.read(&result_[iDiff][iEntry].score, sizeof(uint32));
				reader.read(&result_[iDiff][iEntry].area, sizeof(uint8));
			}
			sort_();
		}
	}

	void ResultList::save(FilePathView dataPath)
	{
		BinaryWriter writer{ dataPath };

		for (int iDiff = 0; iDiff < 3; ++iDiff)
		{
			for (int iEntry = 0; iEntry < EntryCount; ++iEntry)
			{
				writer.write(&result_[iDiff][iEntry].score, sizeof(uint32));
				writer.write(&result_[iDiff][iEntry].area, sizeof(uint8));
			}
		}
	}

	void ResultList::add(Difficulty difficulty, const GameResult& entry)
	{
		auto& result = result_[FromEnum(difficulty)];

		result.push_back(entry);
		sort_();

		if (result.size() > EntryCount)
		{
			const auto nOver = result.size() - EntryCount;
			result.pop_back_N(nOver);
		}
	}

	void ResultList::sort_()
	{
		for (auto& r : result_)
		{
			r.sort_by([](const auto& a, const auto& b) { return a.score > b.score; });
		}
	}
}
