#include "debug_output.h"
#include "image.h"
#include "light.h"
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
  // position, radius, surface color, reflectivity, transparency
  spheres.push_back(std::make_unique<Sphere>(Vec3f(0.0f, -10004.f, -20.f),
                                             10000, Vec3f(0.20f, 0.20f, 0.20f),
                                             0.0, 0.0));
  spheres.push_back(std::make_unique<Sphere>(
      Vec3f(0.0f, 0.f, -20.f), 4, Vec3f(1.00f, 0.32f, 0.36f), 1.f, 1.0f));
  spheres.push_back(std::make_unique<Sphere>(
      Vec3f(5.0f, -1.f, -15.f), 2, Vec3f(0.90f, 0.76f, 0.46f), .70, .70));
  spheres.push_back(std::make_unique<Sphere>(
      Vec3f(5.0f, 0.f, -25.f), 3, Vec3f(0.65f, 0.77f, 0.97f), .50, .0));
  spheres.push_back(std::make_unique<Sphere>(
      Vec3f(-5.5f, 0.f, -15.f), 3, Vec3f(0.10f, 0.10f, 0.10f), .50, 0.0));

  // light
  LightsCollection lights;
  lights.push_back(std::make_unique<PointLight>(Vec3f(5.0f, 20.f, -30.f),
                                                Vec3f(1.f), 2000));
  // lights.push_back(std::make_unique<DistantLight>(
  //   Vec3f(-1.0f, -1.f, -1.f).normalize(), Vec3f(1.f), 1));

  auto objGeneration{std::chrono::high_resolution_clock::now()};

  Render(spheres, lights, image);
  auto renderTS{std::chrono::high_resolution_clock::now()};

  // SaveToFile("./untitled.ppm", image);
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
