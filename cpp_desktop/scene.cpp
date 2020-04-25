#include "debug_output.h"
#include "image.h"
#include "light.h"
#include "photon_map.h"
#include "render.h"
#include "sphere.h"
#include "triangle_mesh.h"
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <vector>

/// World coordinates are consistent with OpenGL
///
///  Front, -Z-axis is deep into screen
///  Front, +Z-axis is pointing to you
///
///   Y
///   ^
///   |
///   |----> X
///
///
///  Looking down from the top, +Y-axis is pointing to you
///
///   -Z
///   ^
///   |
///   |----> X
///
///
///  Looking from right to left, +X-axis is pointing to you
///
///   Y
///   ^
///   |
///   |----> -Z

/// In the main function, we will create the scene which is composed of 5
/// spheres and 1 light (which is also a sphere). Then, once the scene
/// description is complete we render that scene, by calling the render()
/// function.
void scene_spheres(Image &image)
{
  std::srand(13);

  auto start{std::chrono::high_resolution_clock::now()};

  ObjectsCollection spheres;
  // position, radius, surface color, reflectivity, transparency
  spheres.push_back(
      std::make_unique<Sphere>(Vec3f(0.0f, -10004.f, -20.f), 10000, Vec3f(0.20f, 0.20f, 0.20f), 0.0, 0.0));

  spheres.push_back(std::make_unique<Sphere>(Vec3f(0.0f, 0.f, -20.f), 4, Vec3f(1.00f, 0.32f, 0.36f), 1.f, 1.0f));
  spheres.push_back(std::make_unique<Sphere>(Vec3f(5.0f, -1.f, -15.f), 2, Vec3f(0.90f, 0.76f, 0.46f), .70, .70));
  spheres.push_back(std::make_unique<Sphere>(Vec3f(5.0f, 0.f, -25.f), 3, Vec3f(0.65f, 0.77f, 0.97f), .50, .0));
  spheres.push_back(std::make_unique<Sphere>(Vec3f(-5.5f, 0.f, -15.f), 3, Vec3f(0.10f, 0.10f, 0.10f), 1.0, 0.0));

  // light
  LightsCollection lights;
  lights.push_back(std::make_unique<PointLight>(Vec3f(5.0f, 20.f, -30.f), Vec3f(1.f), 2000));
  // lights.push_back(std::make_unique<DistantLight>(
  //   Vec3f(-1.0f, -1.f, -1.f).normalize(), Vec3f(1.f), 1));

  auto objGeneration{std::chrono::high_resolution_clock::now()};

  const View view{Vec3f{0.f, 20.f, 40.f}, Vec3f(0, -0.5f, -20), image.width, image.height, 15.f};

  //const PhotonMap map = MakePhotonMap(view.cameraCoordinates, spheres, lights);

  Render(view, spheres, lights, image);

  //auto inverseCamera = view.cameraCoordinates.inverse();
  //for (auto &photon : map)
  //{
  //  auto p2d = inverseCamera.multVecMatrix(photon.hit);
  //  p2d      = InvertPerspective(view, p2d);
  //  if (std::abs(p2d.x) >= image.width || std::abs(p2d.y) >= image.height || p2d.x < 0 || p2d.y < 0)
  //    continue;
  //  image.Pixel((int)p2d.x, (int)p2d.y, Vec3f{1.f, 1.f, 1.f});
  //}

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

void scene_mesh(Image &image)
{
  std::srand(13);

  auto start{std::chrono::high_resolution_clock::now()};

  auto mesh{LoadPolyMeshFromFile("./geometry/glasses.geo", IdentityMtx44f)};
  auto tt = DeltaTimeMilisec(std::chrono::high_resolution_clock::now(), start);
  if (!mesh)
  {
    OutputDebugString("Failed to load mesh\n");
    return;
  }
  else
    OutputDebugString("Load Time: " + std::to_string(tt) + "ms\n");

  ObjectsCollection objects;
  objects.push_back(std::move(mesh));

  // light
  LightsCollection lights;
  lights.push_back(std::make_unique<PointLight>(Vec3f(5.0f, 20.f, -30.f), Vec3f(1.f), 2000));
  // lights.push_back(std::make_unique<DistantLight>(
  //   Vec3f(-1.0f, -1.f, -1.f).normalize(), Vec3f(1.f), 1));

  auto objGeneration{std::chrono::high_resolution_clock::now()};

  const View view{Vec3f{0.f, 0.f, 40.f}, Vec3f{0.f, 0.f, -1}, image.width, image.height, 15.f};
  Render(view, objects, lights, image);
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

void scene(Image &image)
{
  scene_spheres(image);
  // scene_mesh(image);
}
