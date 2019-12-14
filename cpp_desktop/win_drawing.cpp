#include <windows.h>
#include <gdiplus.h>
#include <objidl.h>
#pragma comment(lib, "Gdiplus.lib")

#include "image.h"
#include <memory>

void DrawImage(Gdiplus::Bitmap &hnd, const Image &image)
{
  int width{0};
  int height{0};
  for (const auto &pixel : image)
  {
    if (width >= image.width)
    {
      width = 0;
      height++;
    }
    hnd.SetPixel(width++, height, Gdiplus::Color(0xFF000000 | Colour(pixel, false)));
  }
}

std::unique_ptr<Gdiplus::Bitmap> g_bitmap;

void OnPaint(HDC &hnd, Image &image)
{
  Gdiplus::Graphics g(hnd);
  if (!g_bitmap)
  {
    g_bitmap = std::make_unique<Gdiplus::Bitmap>(image.width, image.height, &g);
    DrawImage(*g_bitmap, image);
  }
  g.DrawImage(g_bitmap.get(), 0, 0);
}

