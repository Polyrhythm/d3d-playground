#include "systemclass.h"
#include "inputclass.h"
#include "graphicsclass.h"

SystemClass::SystemClass()
    : _input(0)
    , _graphics(0)
{
}

SystemClass::SystemClass(const SystemClass& other)
{
}

SystemClass::~SystemClass()
{
}

bool SystemClass::Initialize()
{
    int screenWidth, screenHeight;
    bool result;

    screenWidth = 0;
    screenHeight = 0;

    InitializeWindows(screenWidth, screenHeight);

    _input = new InputClass;
    if (!_input)
    {
        return false;
    }

    _input->Initialize();

    _graphics = new GraphicsClass;
    if (!_graphics)
    {
        return false;
    }

    result = _graphics->Initialize(screenWidth, screenHeight, _hwnd);
    if (!result)
    {
        return false;
    }

    return true;
}

void SystemClass::Shutdown()
{
    if (_graphics)
    {
        _graphics->Shutdown();
        delete _graphics;
        _graphics = 0;
    }

    if (_input)
    {
        delete _input;
        _input = 0;
    }

    ShutdownWindows();
}

void SystemClass::Run()
{
    MSG msg;
    bool done, result;

    ZeroMemory(&msg, sizeof(MSG));

    done = false;
    while (!done) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE));
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (msg.message == WM_QUIT)
        {
            done = true;
        }
        else
        {
            result = Frame();
            if (!result)
            {
                done = true;
            }
        }
    }
}

bool SystemClass::Frame()
{
    bool result;

    if (_input->IsKeyDown(VK_ESCAPE))
    {
        return false;
    }

    result = _graphics->Frame();
    if (!result)
    {
        return false;
    }

    return true;
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam,
    LPARAM lparam)
{
    switch (umsg)
    {
        case WM_KEYDOWN:
        {
            _input->KeyDown((unsigned int)wparam);
            return 0;
        }

        case WM_KEYUP:
        {
            _input->KeyUp((unsigned int)wparam);
            return 0;
        }

        default:
        {
            return DefWindowProc(hwnd, umsg, wparam, lparam);
        }
    }
}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
    WNDCLASSEX wc;
    DEVMODE dmScreenSettings;
    int posX, posY;

    ApplicationHandle = this;
    _hInstance = GetModuleHandle(NULL);
    _applicationName = L"Engine";

    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = _hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    wc.hIconSm = wc.hIcon;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = _applicationName;
    wc.cbSize = sizeof(WNDCLASSEX);

    RegisterClassEx(&wc);

    screenWidth = GetSystemMetrics(SM_CXSCREEN);
    screenHeight = GetSystemMetrics(SM_CYSCREEN);

    if (FULL_SCREEN)
    {
        memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
        dmScreenSettings.dmSize = sizeof(dmScreenSettings);
        dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
        dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
        dmScreenSettings.dmBitsPerPel = 32;
        dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

        posX = posY = 0;
    }
    else
    {
        screenWidth = 800;
        screenHeight = 600;

        posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
        posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
    }

    _hwnd = CreateWindowEx(WS_EX_APPWINDOW, _applicationName, _applicationName,
        WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, posX, posY, screenWidth,
        screenHeight, NULL, NULL, _hInstance, NULL);

    ShowWindow(_hwnd, SW_SHOW);
    SetForegroundWindow(_hwnd);
    SetFocus(_hwnd);

    ShowCursor(false);

    return;
}

void SystemClass::ShutdownWindows()
{
    ShowCursor(true);

    if (FULL_SCREEN)
    {
        ChangeDisplaySettings(NULL, 0);
    }

    DestroyWindow(_hwnd);
    _hwnd = NULL;

    UnregisterClass(_applicationName, _hInstance);
    _hInstance = NULL;

    ApplicationHandle = NULL;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
    switch (umessage)
    {
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }

        case WM_CLOSE:
        {
            PostQuitMessage(0);
            return 0;
        }

        default:
        {
            return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
        }
    }
}