#include "graphicsclass.h"

GraphicsClass::GraphicsClass()
    : _Direct3D(0)
{

}

GraphicsClass::GraphicsClass(const GraphicsClass& other)
{

}

GraphicsClass::~GraphicsClass()
{

}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
    bool result;

    _Direct3D = new D3DClass;
    if (!_Direct3D)
    {
        return false;
    }

    result = _Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED,
        hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
        return false;
    }

    return true;
}

void GraphicsClass::Shutdown()
{
    if (_Direct3D)
    {
        _Direct3D->Shutdown();
        delete _Direct3D;
        _Direct3D = 0;
    }

    return;
}

bool GraphicsClass::Frame()
{
    bool result;

    result = Render();
    if (!result)
    {
        return false;
    }

    return true;
}

bool GraphicsClass::Render()
{
    _Direct3D->BeginScene(1.0f, 1.0f, 0.0f, 1.0f);
    _Direct3D->EndScene();

    return true;
}


