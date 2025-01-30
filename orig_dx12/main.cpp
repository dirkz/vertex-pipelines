#include "stdafx.h"

#include "DXUltra.h"
#include "Shader.h"
#include "Strings.h"

using namespace dxultra;

static int APIENTRY WinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE hInstPrev, _In_ PSTR cmdline,
                            _In_ int cmdshow)
{
    OutputDebugString(L"Hello, DX Ultra!\n");

    int returnValue = 0;

    {
        DXUltra window{};

#ifdef _DEBUG
        {
            std::wstring moduleFileName = Shader::ModuleFileName();
            std::wstring output = std::format(L"*** module filename {}\n", moduleFileName);
            OutputDebugString(output.c_str());

            output = std::format(L"*** module path {}\n", Shader::ModulePath());
            OutputDebugString(output.c_str());

            output = std::format(L"*** shader base path {}\n", Shader::ShaderSourceBasePath());
            OutputDebugString(output.c_str());

            std::string s = WideCharToUTF8(output);
            s = WideCharToUTF8(L"Message with üצה\n");
            OutputDebugStringA(s.c_str()); // this will be garbage, but is expected
        }
#endif

        returnValue = Window::Run(&window, hInst, hInstPrev, cmdshow);
    }

    return returnValue;
}