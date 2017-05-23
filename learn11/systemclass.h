#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

//#include "inputclass.h"
//#include "graphicsclass.h"

class InputClass;
class GraphicsClass;

class SystemClass
{
public:
    SystemClass();
    SystemClass(const SystemClass&);
    ~SystemClass();

    bool Initialize();
    void Shutdown();
    void Run();

    LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
    bool Frame();
    void InitializeWindows(int&, int&);
    void ShutdownWindows();

    LPCWSTR _applicationName;
    HINSTANCE _hInstance;
    HWND _hwnd;

    InputClass* _input;
    GraphicsClass* _graphics;
};

/*
    Function prototypes
*/
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

/*
    Globals
*/
static SystemClass* ApplicationHandle = 0;

#endif
