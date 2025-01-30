#include "Window.h"

namespace dxultra
{

LRESULT CALLBACK Window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    Window *pWindow = reinterpret_cast<Window *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

    if (!pWindow)
    {
        switch (uMsg)
        {
        case WM_NCCREATE: {
            LPCREATESTRUCT pCreateStruct = reinterpret_cast<CREATESTRUCT *>(lParam);
            pWindow = reinterpret_cast<Window *>(pCreateStruct->lpCreateParams);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
            return TRUE;
        }
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }
    else
    {
        return pWindow->HandleMessage(hwnd, uMsg, wParam, lParam);
    }
}

int Window::Run(Window *pWindow, HINSTANCE hInst, HINSTANCE hInstPrev, int nCmdShow)
{
    RECT winRect = pWindow->PreferredWindowsRect();

    WNDCLASSEX wndClass{};
    wndClass.cbSize = sizeof(wndClass);
    wndClass.lpfnWndProc = WindowProc;
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.hInstance = hInst;
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.lpszClassName = L"DXUltraWindowClass";

    ATOM atom = RegisterClassEx(&wndClass);
    if (!atom)
    {
        CheckLastError();
    }

    DWORD dwStyle = WS_OVERLAPPEDWINDOW;
    DWORD dwExStyle = WS_EX_APPWINDOW;

    HWND hwnd = CreateWindowEx(dwExStyle, wndClass.lpszClassName, pWindow->Title().c_str(), dwStyle,
                               winRect.left, winRect.top, winRect.right - winRect.left,
                               winRect.bottom - winRect.top, nullptr, nullptr, hInst, pWindow);

    if (!hwnd)
    {
        CheckLastError();
        return 0;
    }
    else
    {
        pWindow->CreatedWindow(hwnd);

        ShowWindow(hwnd, nCmdShow);

        MSG msg = {};
        while (GetMessage(&msg, NULL, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        return msg.wParam;
    }
}

RECT Window::PreferredWindowsRect()
{
    LONG width = GetSystemMetrics(SM_CXSCREEN);
    LONG height = GetSystemMetrics(SM_CYSCREEN);
    LONG winLeft = width / 4;
    LONG winWidth = width / 2;

    return RECT{winLeft, 0, winLeft + winWidth, height / 2};
}

std::wstring Window::Title()
{
    return L"Hello, Window!";
}

long long Window::ActiveTime() const
{
    auto now = std::chrono::steady_clock::now();
    long long wholeElapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(now - m_firstTimeActivated).count();
    return wholeElapsed - m_totalElapsedInactiveTime;
}

void Window::SetWindowDebugText(const std::wstring &string)
{
    m_windowDebugText = string;
}

LRESULT Window::HandleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        WillDestroyWindow(hwnd);
        return 0;

    case WM_ENTERSIZEMOVE:
        m_isActivelyResizing = true;
        return 0;

    case WM_EXITSIZEMOVE:
        m_isActivelyResizing = false;
        ResizeWindow(hwnd, m_width, m_height);
        return 0;

    case WM_SIZE:
        m_width = LOWORD(lParam);
        m_height = HIWORD(lParam);

        if (!m_isActivelyResizing)
        {
            ResizeWindow(hwnd, m_width, m_height);
        }

        return 0;

    case WM_PAINT: {
        long long totalElapsedActiveTime = ActiveTime();

        Update(totalElapsedActiveTime);
        DrawWindow(hwnd);
        UpdateWindowText(hwnd, totalElapsedActiveTime);

        return 0;
    }

    case WM_ACTIVATE:
        auto now = std::chrono::steady_clock::now();

        WORD state = LOWORD(wParam);
        if (state == WA_INACTIVE)
        {
            // Deactivate
            m_lastTimeDeactivated = now;
        }
        else
        {
            // Activate
            if (m_lastTimeDeactivated != m_defaultTime)
            {
                auto elapsedInactive = std::chrono::duration_cast<std::chrono::milliseconds>(
                                           now - m_lastTimeDeactivated)
                                           .count();
                m_totalElapsedInactiveTime += elapsedInactive;
            }
            else
            {
                m_firstTimeActivated = now;
            }
        }

        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void Window::UpdateWindowText(HWND hwnd, long long totalElapsedActiveTime)
{
    std::wstring title = Title();
    title += std::format(L" {} {}", totalElapsedActiveTime, m_windowDebugText);
    SetWindowText(hwnd, title.c_str());
}

} // namespace dxultra