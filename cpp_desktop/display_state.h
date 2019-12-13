#ifndef DISPLAY_STATE_H
#define DISPLAY_STATE_H

#include "framework.h"

void WndRestore(const HWND &hwnd, const DWORD &dwStyle);
void WndResizeFullScreen(const HWND &hwnd, const DWORD &dwStyle);
bool SetFullScreen(int width, int height);
bool ExitFullScreen();

#endif
