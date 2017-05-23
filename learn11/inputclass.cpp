#include "inputclass.h"

InputClass::InputClass()
{
}

InputClass::InputClass(const InputClass& other)
{

}

InputClass::~InputClass()
{

}

void InputClass::Initialize()
{
    // initialize all the keys to being released and not pressed
    for (int i = 0; i < 256; i++) {
        _keys[i] = false;
    }
}

void InputClass::KeyDown(unsigned int input)
{
    _keys[input] = true;
}

void InputClass::KeyUp(unsigned int input)
{
    _keys[input] = false;
}

bool InputClass::IsKeyDown(unsigned int key)
{
    return _keys[key];
}
