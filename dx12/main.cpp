#include "stdafx.h"

#include "WindowCallback.h"
#include "Window.h"
#include "DX12Window.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
                    _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
    OutputDebugString(L"Hello, world!\n");

    int returnValue = 0;
    {
        zdx::DX12Window dxWindow{};
        zdx::Window window{&dxWindow};
        returnValue = window.WinMain(hInstance, hPrevInstance, lpCmdLine, nShowCmd);
    }

    return returnValue;
}
