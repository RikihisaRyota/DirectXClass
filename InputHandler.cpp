#include "InputHandler.h"

#include "ICommand.h"
#include "Input.h"

ICommand* InputHandler::HandleInput() {
    if (Input::GetInstance()->TriggerKey(DIK_D)){
        return pressKeyD_;
    }
    if (Input::GetInstance()->TriggerKey(DIK_A)) {
        return pressKeyA_;
    }
    return nullptr;
}

void InputHandler::PressKeyA() {
    ICommand* command = new MoveLeftCommand();
    pressKeyA_ = command;
}

void InputHandler::PressKeyD() {
    ICommand* command = new MoveRightCommand();
    pressKeyD_ = command;
}
