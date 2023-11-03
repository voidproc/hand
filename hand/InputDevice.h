#pragma once

namespace hand
{
	class InputDevice
	{
	public:
		InputDevice(Config& config);

		InputGroup left() const;
		InputGroup right() const;
		InputGroup up() const;
		InputGroup down() const;
		InputGroup action() const;
		InputGroup pause() const;
		InputGroup decide() const;

		bool leftPressed() const;
		bool rightPressed() const;
		bool upPressed() const;
		bool downPressed() const;
		bool leftDown() const;
		bool rightDown() const;
		bool upDown() const;
		bool downDown() const;
		bool leftUp() const;
		bool rightUp() const;
		bool upUp() const;
		bool downUp() const;

		void update();

	private:
		const s3d::detail::XInput_impl& xinput_() const;

		Config& config_;

		InputGroup inputLeft_;
		InputGroup inputRight_;
		InputGroup inputUp_;
		InputGroup inputDown_;
		InputGroup inputAction_;
		InputGroup inputPause_;

		bool prevLeftPressed_;
		bool prevRightPressed_;
		bool prevUpPressed_;
		bool prevDownPressed_;

	};
}
