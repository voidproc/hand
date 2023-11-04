#include "EndingScene.h"
#include "SceneSize.h"
#include "Theme.h"
#include "Hiscore.h"
#include "MsgData.h"
#include "DebugMode.h"

namespace hand
{
	EndingScene::EndingScene(const InitData& init)
		:
		IScene{ init },
		time_{ StartImmediately::Yes, GlobalClock::Get() },
		timerMsg_{ 6.0s, StartImmediately::No, GlobalClock::Get() },
		msgRange_{},
		msg_{}
	{
#ifdef DEBUG_MODE
		//getData().endingType = 1;
#endif

		SaveHiscore(getData());

		if (getData().endingType == 0)
		{
			msgRange_ = std::make_pair(3, 5);
		}
		else
		{
			msgRange_ = std::make_pair(6, 9);
		}

		msg_ = msgRange_.first;
	}

	void EndingScene::update()
	{
		if (timerMsg_.reachedZero())
		{
			if (msg_ < msgRange_.second)
			{
				++msg_;
				timerMsg_.restart();
			}
		}

		if (time_ > 2s && not timerMsg_.isRunning() && msg_ < msgRange_.second)
		{
			timerMsg_.restart();
		}


		// BGM制御

		if (time_ > 3.5s && not isFinishedMsg_() && not AudioAsset(U"Ending").isPlaying())
		{
			AudioAsset(U"Ending").setLoop(true);
			AudioAsset(U"Ending").play(MixBus1, 2s);
		}

		if (isFinishedMsg_() && AudioAsset(U"Ending").isPlaying())
		{
			AudioAsset(U"Ending").stop(3s);
		}


		if (isFinishedMsg_())
		{
			if (getData().input.decide().down())
			{
				changeScene(U"TitleScene", 0s);

				AudioAsset(U"Select").playOneShot();
			}
		}
	}

	void EndingScene::draw() const
	{
		SceneRect.draw(Theme::White);

		constexpr int TopBandHeight = 24;
		constexpr int BottomBandHeight = 40;

		const auto topBandRect = Rect{ 0, 0, SceneWidth, TopBandHeight }.draw(Theme::Lighter);
		FontAsset(U"H68Thin")(U"- EPILOGUE -").drawAt(topBandRect.center() + Vec2{ 0, 1 }, Theme::Darker);
		FontAsset(U"H68Thin")(U"- EPILOGUE -").drawAt(topBandRect.center() + Vec2{ 0, 0 }, Theme::Black);

		if (getData().endingType == 1)
		{
			TextureAsset(U"EndBg1").draw(0, TopBandHeight, AlphaF(0.5 * Clamp((time_.sF() - 4.0) / 5.0, 0.0, 1.0)));
		}

		if (timerMsg_ < 5s)
		{
			const auto text = Msg::Data[msg_];
			const Vec2 textPos = Vec2{ SceneWidth / 2, TopBandHeight + (SceneHeight - (TopBandHeight + BottomBandHeight)) / 2 };
			const double moveY = 6 * (1.0 - EaseOutCubic(Clamp((4.0 - timerMsg_.sF()) / 0.8, 0.0, 1.0)));
			const double alpha = Clamp((4.0 - timerMsg_.sF()) / 0.4, 0.0, 1.0);
			const double alpha2 = (timerMsg_ > 1s || msg_ == msgRange_.second) ? 1.0 : static_cast<int>(4.0 * Clamp(timerMsg_.sF() / 1.0, 0.0, 1.0)) / 4.0;

			FontAsset(U"Sub")(text).drawAt(textPos + Vec2{ 0, 1 } + Vec2{ 0, moveY }, ColorF{ Theme::Lighter, alpha * alpha2 });
			FontAsset(U"Sub")(text).drawAt(textPos + Vec2{ 0, 0 } + Vec2{ 0, moveY }, ColorF{ Theme::Darker, alpha * alpha2 });
		}

		const auto bottomBandRect = Rect{ 0, SceneHeight - BottomBandHeight, SceneWidth, BottomBandHeight }.draw(Theme::Lighter);

		if (isFinishedMsg_())
		{
			const auto text = U"THANKS FOR PLAYING!"_fmt();
			FontAsset(U"H68Thin")(text).drawAt(bottomBandRect.center() + Vec2{ 0, -8 + 1 }, Theme::Darker.lerp(Theme::Lighter, 0.7 + 0.3 * Periodic::Sine0_1(0.2s)));
			FontAsset(U"H68Thin")(text).drawAt(bottomBandRect.center() + Vec2{ 0, -8 + 0 }, Theme::Black.lerp(Theme::Darker, 0.7 + 0.3 * Periodic::Sine0_1(0.2s)));

			const auto textScore = U"{:08d}"_fmt(getData().score);
			FontAsset(U"H68")(textScore).drawAt(bottomBandRect.center() + Vec2{ 0, 6 + 1 }, Theme::Darker);
			FontAsset(U"H68")(textScore).drawAt(bottomBandRect.center() + Vec2{ 0, 6 + 0 }, Theme::Black);
		}

		SceneRect.drawFrame(2, 0, ColorF{ Theme::Black, 0.5 });

		// フェードイン
		if (time_ < 0.5s)
		{
			SceneRect.draw(ColorF{ Theme::White, 1.0 - Clamp(time_.sF() / 0.5, 0.0, 1.0)});
		}
	}

	bool EndingScene::isFinishedMsg_() const
	{
		return timerMsg_.reachedZero() && msg_ == msgRange_.second;
	}
}
