#ifndef IMAGE_H
#define IMAGE_H

#include <algorithm>
#include <fstream>
#include <ostream>
#include <string>
#include <vector>

#include "geometry.h"

class Image
{
public:
  const int width;
  const int height;


  Image(int width_, int height_)
      : width{width_}
      , height{height_}
      , pixels(width * height)
  {
  }

  const Vec3f &operator[](int idx) const { return pixels[idx]; }
  Vec3f &operator[](int idx) { return pixels[idx]; }

  auto begin() { return std::begin(pixels); }
  const auto begin() const { return std::begin(pixels); }
  auto end() { return std::end(pixels); }
  const auto end() const { return std::end(pixels); }

  friend void SaveToFile(std::string filename, const Image& i)
  {
    // Save result to a PPM image (keep these flags if you compile under
    // Windows)
    std::ofstream ofs(filename, std::ios::out | std::ios::binary);
    ofs << "P6\n" << i.width << " " << i.height << "\n255\n";

    for (const auto &pxl : i.pixels)
    {
      ofs << (unsigned char)(std::min(1.f, pxl.x) * 255)
          << (unsigned char)(std::min(1.f, pxl.y) * 255)
          << (unsigned char)(std::min(1.f, pxl.z) * 255);
    }

    ofs.close();
  }

private:
  std::vector<Vec3f> pixels;
};

inline int32_t Colour(const Vec3f &colour)
{
  int32_t r = static_cast<unsigned char>(std::min(1.f, colour.x) * 255);
  int32_t g = static_cast<unsigned char>(std::min(1.f, colour.y) * 255);
  int32_t b = static_cast<unsigned char>(std::min(1.f, colour.z) * 255);

  return (b << 16) | (g << 8) | r;
}

#endif // !IMAGE_H
