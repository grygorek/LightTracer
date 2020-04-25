#include "photon_map.h"
#include "geometry.h"
#include <random>
#include <vector>

PhotonMap MakePhotonMap(const Matrix44f &camera, const ObjectsCollection &objects, const LightsCollection &lights)
{
  if (objects.empty() || lights.empty())
    return PhotonMap{};

  constexpr int photonsCnt = 1'000'000;
  std::random_device rndDev;
  std::uniform_real_distribution<double> rnd(
      -1, std::nextafter(1.0, std::numeric_limits<double>::max())); // range [-1,1] inclusive

  PhotonMap map(photonsCnt);
  auto idx{std::begin(map)};
  while (idx != std::end(map))
  {
    Vec3f src = lights[0]->Position();
    Vec3f dir = Vec3f(rnd(rndDev), rnd(rndDev), rnd(rndDev)).normalize();
    auto near = FindNearestObject(src, dir, objects);
    if (near.obj == nullptr)
      continue;

    *idx = Photon{src + near.hit_distance * dir, dir};
    ++idx;
  }

  return map;
}

// 1. How to visualise 3D photons in the 2D camera view?
//
//    2D screen pixel and camera create a view point and ray vector.
//    The problem with visualization is reversed: there is a 3D point and we need to cast it to the screen.
//

// 2. How to find all the 3D photons within the range of a 3D point?
//
//    Variable number of photons within the point's range will help to calculate the rays density needed for caustics.
//
