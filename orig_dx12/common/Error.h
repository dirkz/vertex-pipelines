#pragma once

#include "stdafx.h"

namespace dxultra
{

inline void ThrowIfFailed(HRESULT hr)
{
    if (FAILED(hr))
    {
        _com_error err(hr);
        LPCTSTR errMsg = err.ErrorMessage();
        OutputDebugString(errMsg);
        OutputDebugString(L"\n");
        int nWideChars = wcslen(errMsg);
        int cbNeeded =
            WideCharToMultiByte(CP_UTF8, 0, errMsg, nWideChars, nullptr, 0, nullptr, nullptr);
        std::string message(cbNeeded + 1, '\0');
        int cbWritten = WideCharToMultiByte(CP_UTF8, 0, errMsg, nWideChars, message.data(),
                                            cbNeeded, nullptr, nullptr);
        throw std::runtime_error{message};
    }
}

inline void CheckLastError()
{
    DWORD error = GetLastError();
    HRESULT hr = HRESULT_FROM_WIN32(error);
    ThrowIfFailed(hr);
}

} // namespace dxultra