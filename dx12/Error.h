#pragma once

#include "stdafx.h"

inline void ThrowIfFailed(const HRESULT hr)
{
    if (!SUCCEEDED(hr))
    {
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
