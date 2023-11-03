#include "InputDevice.h"

namespace hand
{
	InputDevice::InputDevice(Config& config)
		: config_{ config }
	{
		inputLeft_ = KeyLeft | KeyA | xinput_().buttonLeft;
		inputRight_ = KeyRight | KeyD | xinput_().buttonRight;
		inputUp_ = KeyUp | KeyW | xinput_().buttonUp;
		inputDown_ = KeyDown | KeyS | xinput_().buttonDown;
		inputAction_ = KeySpace | xinput_().buttonA;
		inputPause_ = KeyEnter | xinput_().buttonStart;

		prevLeftPressed_ = prevRightPressed_ = prevUpPressed_ = prevDownPressed_ = false;
	}

	InputGroup InputDevice::left() const
	{
		return inputLeft_;
	}

	InputGroup InputDevice::right() const
	{
		return inputRight_;
	}

	InputGroup InputDevice::up() const
	{
		return inputUp_;
	}

	InputGroup InputDevice::down() const
	{
		return inputDown_;
	}

	InputGroup InputDevice::action() const
	{
		return inputAction_;
	}

	InputGroup InputDevice::pause() const
	{
		return inputPause_;
	}

	InputGroup InputDevice::decide() const
	{
		return action() | pause();
	}

	const s3d::detail::XInput_impl& InputDevice::xinput_() const
	{
		return XInput(config_.controllerId);
	}

	bool InputDevice::leftPressed() const
	{
		return left().pressed() || xinput_().leftThumbX < -0.5;
	}

	bool InputDevice::rightPressed() const
	{
		return right().pressed() || xinput_().leftThumbX > 0.5;
	}

	bool InputDevice::upPressed() const
	{
		return up().pressed() || xinput_().leftThumbY > 0.5;
	}

	bool InputDevice::downPressed() const
	{
		return down().pressed() || xinput_().leftThumbY < -0.5;
	}

	bool InputDevice::leftDown() const
	{
		return not prevLeftPressed_ && leftPressed();
	}

	bool InputDevice::rightDown() const
	{
		return not prevRightPressed_ && rightPressed();
	}

	bool InputDevice::upDown() const
	{
		return not prevUpPressed_ && upPressed();
	}

	bool InputDevice::downDown() const
	{
		return not prevDownPressed_ && downPressed();
	}

	bool InputDevice::leftUp() const
	{
		return prevLeftPressed_ && not leftPressed();
	}

	bool InputDevice::rightUp() const
	{
		return prevRightPressed_ && not rightPressed();
	}

	bool InputDevice::upUp() const
	{
		return prevUpPressed_ && not upPressed();
	}

	bool InputDevice::downUp() const
	{
		return prevDownPressed_ && not downPressed();
	}


	void InputDevice::update()
	{
		prevLeftPressed_ = leftPressed();
		prevRightPressed_ = rightPressed();
		prevUpPressed_ = upPressed();
		prevDownPressed_ = downPressed();
	}
}
