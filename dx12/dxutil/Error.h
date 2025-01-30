#pragma once

#include "stdafx.h"

inline void ThrowIfFailed(const HRESULT hr)
{
    if (FAILED(hr))
    {
        _com_error err(hr);
        LPCTSTR errMsg = err.ErrorMessage();
        OutputDebugString(errMsg);
        OutputDebugString(L"\n");

        constexpr size_t maxLen = 13;
        char buffer[maxLen];
        int res = sprintf_s(buffer, maxLen, "HRESULT %04X", hr);
        if (res != -1)
        {
            throw std::runtime_error{buffer};
        }
        else
        {
            throw std::runtime_error{"unsuccessful HRESULT"};
        }
    }
}

inline void CheckLastError()
{
    DWORD error = GetLastError();
    HRESULT hr = HRESULT_FROM_WIN32(error);
    ThrowIfFailed(hr);
}