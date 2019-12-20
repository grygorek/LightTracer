#pragma once

#include <ostream>
#include <fstream>
#include <chrono>
#include "debug_output.h"
#include "geometry.h"
#include "triangle_mesh.h"
#include "image.h"

inline void triangle_test()
{
  auto start{std::chrono::high_resolution_clock::now()};

  Vec3f v0(-1, -1, -5);
  Vec3f v1(1, -1, -5);
  Vec3f v2(0, 1, -5);

  const auto width   = 640;
  const auto height  = 480;
  Vec3f cols[3]          = {{0.6f, 0.4f, 0.1f}, {0.1f, 0.5f, 0.3f}, {0.1f, 0.3f, 0.7f}};
  Image pix(width, height);
  float fov              = 51.52f;
  float scale            = tanf(radians(fov * 0.5f));
  float imageAspectRatio = width / (float)height;
  Vec3f orig(0);
  int pixIdx{};
  for (auto j = 0; j < height; ++j)
  {
    for (auto i = 0; i < width; ++i)
    {
      // compute primary ray
      float x = static_cast<float>((2 * (i + 0.5) / (float)width - 1) * imageAspectRatio * scale);
      float y = static_cast<float>((1 - 2 * (j + 0.5) / (float)height) * scale);
      Vec3f dir(x, y, -1);
      dir.normalize();
      float t, u, v;
      if (rayTriangleIntersect(orig, dir, v0, v1, v2, t, u, v))
      {
        // Interpolate colors using the barycentric coordinates
        pix[pixIdx] = u * cols[0] + v * cols[1] + (1 - u - v) * cols[2];
        // uncomment this line if you want to visualize the row barycentric
        // coordinates
        //pix[pixIdx] = Vec3f(u, v, 1 - u - v);
      }
      pixIdx++;
    }
  }
  auto end{std::chrono::high_resolution_clock::now()};

  OutputDebugString("Triangle test: " + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count()) + "ms\n");

  SaveToFile("./triangle_test.ppm", pix);
}
