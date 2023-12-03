#pragma once

class ICommand;
class InputHandler {
public:
	ICommand* HandleInput();

	void PressKeyA();
	void PressKeyD();

private:
	ICommand* pressKeyA_;
	ICommand* pressKeyD_;
};

