#include "display_state.h"

WINDOWPLACEMENT g_wpPrev = {sizeof(g_wpPrev)};

void WndRestore(const HWND &hwnd, const DWORD &dwStyle)
{
  SetWindowLong(hwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
  SetWindowPlacement(hwnd, &g_wpPrev);
  SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
               SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER |
                   SWP_FRAMECHANGED);
}

void WndResizeFullScreen(const HWND &hwnd, const DWORD &dwStyle)
{
  MONITORINFO mi = {sizeof(mi)};
  if (GetWindowPlacement(hwnd, &g_wpPrev) &&
      GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY), &mi))
  {
    SetWindowLong(hwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
    SetWindowPos(hwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top,
                 mi.rcMonitor.right - mi.rcMonitor.left,
                 mi.rcMonitor.bottom - mi.rcMonitor.top,
                 SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
  }
}

bool SetFullScreen(int width, int height)
{
  DEVMODE mode{};
  if (!EnumDisplaySettings(nullptr, 0, &mode))
    return false;

  mode.dmPelsWidth        = width;
  mode.dmPelsHeight       = height;
  mode.dmDisplayFrequency = 60;

  // Change display mode.
  auto r =
      ChangeDisplaySettingsEx(nullptr, &mode, nullptr, CDS_FULLSCREEN, nullptr);

  return r == DISP_CHANGE_SUCCESSFUL;
}

bool ExitFullScreen()
{
  // Restore default display mode.
  auto r = ChangeDisplaySettingsEx(nullptr, nullptr, nullptr, 0, nullptr);
  return r == DISP_CHANGE_SUCCESSFUL;
}
