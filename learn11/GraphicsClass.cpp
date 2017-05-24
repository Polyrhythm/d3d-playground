#include "graphicsclass.h"

GraphicsClass::GraphicsClass()
    : _Direct3D(0)
    , _Camera(0)
    , _Model(0)
    , _ColorShader(0)
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

    _Camera = new CameraClass;
    if (!_Camera)
    {
        return false;
    }
    _Camera->SetPosition(0.0f, 0.0f, -5.0f);

    _Model = new ModelClass;
    if (!_Model)
    {
        return false;
    }

    result = _Model->Initialize(_Direct3D->GetDevice());
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the model object",
            L"Error", MB_OK);
        return false;
    }

    _ColorShader = new ColorShaderClass;
    if (!_ColorShader)
    {
        MessageBox(hwnd, L"Could not initialize the color shader",
            L"Error", MB_OK);
        return false;
    }

    return true;
}

void GraphicsClass::Shutdown()
{
    if (_ColorShader)
    {
        _ColorShader->Shutdown();
        delete _ColorShader;
        _ColorShader = 0;
    }

    if (_Model)
    {
        _Model->Shutdown();
        delete _Model;
        _Model = 0;
    }

    if (_Camera)
    {
        delete _Camera;
        _Camera = 0;
    }

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
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
    bool result;

    _Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

    _Camera->Render();

    _Direct3D->GetWorldMatrix(worldMatrix);
    _Camera->GetViewMatrix(viewMatrix);
    _Direct3D->GetProjectionMatrix(projectionMatrix);

    _Model->Render(_Direct3D->GetDeviceContext());

    result = _ColorShader->Render(_Direct3D->GetDeviceContext(),
        _Model->GetIndexCount(), worldMatrix, viewMatrix,
        projectionMatrix);
    if (!result)
    {
        return false;
    }

    _Direct3D->EndScene();

    return true;
}


