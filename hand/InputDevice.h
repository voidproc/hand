#pragma once

namespace hand
{
	class InputDevice
	{
	public:
		InputDevice();

		InputGroup left() const;
		InputGroup right() const;
		InputGroup up() const;
		InputGroup down() const;
		InputGroup action() const;
		InputGroup pause() const;
		InputGroup decide() const;


	private:
		InputGroup inputLeft_;
		InputGroup inputRight_;
		InputGroup inputUp_;
		InputGroup inputDown_;
		InputGroup inputAction_;
		InputGroup inputPause_;
	};
}
