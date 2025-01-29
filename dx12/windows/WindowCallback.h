#pragma once

#include "stdafx.h"

namespace zdx
{

struct WindowCallback
{
    virtual void OnInit(HWND hwnd, UINT width, UINT height) = 0;
    virtual void OnUpdate() = 0;
    virtual void OnRender() = 0;
};

} // namespace zdx
