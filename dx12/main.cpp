#include "stdafx.h"

#include "Window.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
                    _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
    OutputDebugString(L"Hello, world!\n");

    int returnValue = 0;
    {
        zdx::Window window{};
        returnValue = window.WinMain(hInstance, hPrevInstance, lpCmdLine, nShowCmd);
    }

    return returnValue;
}
