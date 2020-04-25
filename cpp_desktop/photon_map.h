#ifndef PHOTON_MAP_H__
#define PHOTON_MAP_H__

#include <vector>
#include "object.h"
#include "render.h"

struct Photon
{
  Vec3f hit;
  Vec3f dir;
};

using PhotonMap = std::vector<Photon>;

PhotonMap MakePhotonMap(const Matrix44f &camera, const ObjectsCollection &objects, const LightsCollection &lights);

#endif // PHOTON_MAP_H__
