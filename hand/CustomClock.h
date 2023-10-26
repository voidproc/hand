#pragma once

namespace hand
{
	class CustomClock : public ISteadyClock
	{
	public:
		CustomClock(StartImmediately startImmediately = StartImmediately::No)
			: m_clock{ startImmediately }
		{
		}

		virtual ~CustomClock() {}

		virtual uint64 getMicrosec() override
		{
			return m_clock.us64();
		}

		void start()
		{
			m_clock.start();
		}

		void pause()
		{
			m_clock.pause();
		}

		bool isPaused() const
		{
			return m_clock.isPaused();
		}

		void setSpeed(const double speed)
		{
			m_clock.setSpeed(speed);
		}

		double getSpeed() const
		{
			return m_clock.getSpeed();
		}

	private:
		VariableSpeedStopwatch m_clock;
	};

	struct GlobalClock
	{
		static CustomClock* Get()
		{
			static CustomClock clock{ StartImmediately::Yes };
			return &clock;
		}
	};
}
