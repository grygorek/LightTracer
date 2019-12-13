#include "debug_output.h"
#include "image.h"
#include "render.h"
#include "sphere.h"
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <vector>

/// In the main function, we will create the scene which is composed of 5
/// spheres and 1 light (which is also a sphere). Then, once the scene
/// description is complete we render that scene, by calling the render()
/// function.
void scene(Image &image)
{
  std::srand(13);

  auto start{std::chrono::high_resolution_clock::now()};

  ObjectsCollection spheres;
  // position, radius, surface color, reflectivity, transparency, emission color
  spheres.push_back(
      std::make_unique<SphereSimple>(Vec3f(0.0f, -10004.f, -20.f), 10000,
                                     Vec3f(0.20f, 0.20f, 0.20f), 0.00, 0.0));
  spheres.push_back(std::make_unique<SphereSimple>(
      Vec3f(0.0f, 0.f, -20.f), 4, Vec3f(1.00f, 0.32f, 0.36f), 0.0, 1.00));
  spheres.push_back(std::make_unique<SphereSimple>(
      Vec3f(5.0f, -1.f, -15.f), 2, Vec3f(0.90f, 0.76f, 0.46f), .02, 0.0));
  spheres.push_back(std::make_unique<SphereSimple>(
      Vec3f(5.0f, 0.f, -25.f), 3, Vec3f(0.65f, 0.77f, 0.97f), 0.02, 0.0));
  spheres.push_back(std::make_unique<SphereSimple>(
      Vec3f(-5.5f, 0.f, -15.f), 3, Vec3f(0.90f, 0.90f, 0.90f), 0.02, 0.0));
  // light
  ObjectsCollection lights;
  lights.push_back(std::make_unique<SphereSimple>(Vec3f(0.0f, 20.f, -30.f), 3,
                                                   Vec3f(0.00f, 0.00f, 0.00f),
                                                   0, 0.0, Vec3f(2.f)));

  auto objGeneration{std::chrono::high_resolution_clock::now()};

  render(spheres, lights, image);
  auto renderTS{std::chrono::high_resolution_clock::now()};

  SaveToFile("./untitled.ppm", image);
  auto saveTS{std::chrono::high_resolution_clock::now()};

  //
  //
  // Statistics

  auto genTime    = objGeneration - start;
  auto renderTime = renderTS - objGeneration;
  auto saveTime   = saveTS - renderTS;

  auto Duration = [](const auto &d) {
    auto s  = std::chrono::duration_cast<std::chrono::seconds>(d);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(d - s);
    auto us = std::chrono::duration_cast<std::chrono::microseconds>(d - s - ms);

    std::stringstream ss;
    ss << s.count() << "s " << ms.count() << "ms " << us.count() << "us";
    return ss.str();
  };

  std::stringstream str;
  str << "Obj Generation Time: " << Duration(genTime) << "\n"
      << "Render Time: " << Duration(renderTime) << "\n"
      << "File Save Time: " << Duration(saveTime) << "\n";
  OutputDebugString(str.str());
}
