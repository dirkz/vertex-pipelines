#pragma once

#include "stdafx.h"

namespace dxultra
{

struct Window
{
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static int Run(Window *pWindow, HINSTANCE hInst, HINSTANCE hInstPrev, int cmdshow);

    virtual RECT PreferredWindowsRect();
    virtual std::wstring Title();

    DWORD Width() const
    {
        return m_width;
    }

    DWORD Height() const
    {
        return m_height;
    }

    double AspectRatio() const
    {
        return static_cast<double>(m_width) / m_height;
    }

    /// <summary>
    /// Total active time since first activation in milliseconds.
    /// </summary>
    /// <returns></returns>
    long long ActiveTime() const;

    /// <summary>
    /// Add the given text to the title on the next <c>WM_PAINT</c>.
    /// </summary>
    /// <param name="string"></param>
    void SetWindowDebugText(const std::wstring &string);

  protected:
    /// <summary>
    /// Called after window creation, right before it gets shown.
    /// </summary>
    /// <param name="hwnd"></param>
    virtual void CreatedWindow(HWND hwnd) {};

    virtual void ResizeWindow(HWND hwnd, DWORD width, DWORD height) {};

    /// <summary>
    /// Called before <c>DrawWindow</c> with a time measurement.
    /// </summary>
    /// <param name="totalElapsedActiveTime">Total elapsed active time in milliseconds</param>
    virtual void Update(long long totalElapsedActiveTime) {};

    virtual void DrawWindow(HWND hwnd) {};
    virtual void WillDestroyWindow(HWND hwnd) {};

    virtual LRESULT HandleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

  private:
    void UpdateWindowText(HWND hwnd, long long totalElapsedActiveTime);

    bool m_isActivelyResizing = false;
    DWORD m_width = 0;
    DWORD m_height = 0;

    std::chrono::steady_clock::time_point m_defaultTime{};
    std::chrono::steady_clock::time_point m_firstTimeActivated{};
    std::chrono::steady_clock::time_point m_lastTimeDeactivated{};
    long long m_totalElapsedInactiveTime = 0;

    std::wstring m_windowDebugText{};
};

} // namespace dxultra