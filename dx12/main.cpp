#include "stdafx.h"

#include "WindowCallback.h"
#include "Window.h"
#include "DXWindow.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
                    _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
    OutputDebugString(L"Hello, world!\n");

    int returnValue = 0;
    {
        zdx::DXWindow dxWindow{};
        zdx::Window window{&dxWindow};
        returnValue = window.WinMain(hInstance, hPrevInstance, lpCmdLine, nShowCmd);
    }

    return returnValue;
}
