#include "InputDevice.h"

namespace hand
{
	InputDevice::InputDevice()
	{
		inputLeft_ = KeyLeft | KeyA;
		inputRight_ = KeyRight | KeyD;
		inputUp_ = KeyUp | KeyW;
		inputDown_ = KeyDown | KeyS;
		inputAction_ = KeySpace;
		inputPause_ = KeyEnter;
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
}
