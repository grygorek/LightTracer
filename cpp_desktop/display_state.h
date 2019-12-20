#ifndef DISPLAY_STATE_H_
#define DISPLAY_STATE_H_

#include <Windows.h>

/// Restore original window size
///
/// @param hwnd - handle to a window
/// @param dwStyle - result of GetWindowLong(hwnd, GWL_STYLE);
void WndRestore(HWND hwnd, DWORD dwStyle);

/// Resize window to screen resolution
void WndResizeFullScreen(HWND hwnd, DWORD dwStyle);

/// Switch screen mode to full screen with provided resolution
///
/// @retval true - switched sucessfully
/// @retval false - failed to switch
///
/// @remarks New resolution must be supported
bool SetFullScreen(int width, int height);

/// Return from full screen to default screen resolution
///
/// @returns true if success, false otherwise
bool ExitFullScreen();

#endif
