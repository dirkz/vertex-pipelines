#pragma once

#include "stdafx.h"

namespace zdx
{

struct WindowCallback
{
    virtual void OnInit(HWND hwnd, UINT width, UINT height) = 0;
};

} // namespace zdx
