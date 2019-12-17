#ifndef MATERIAL_DEF_H
#define MATERIAL_DEF_H

#include "geometry.h"

enum class MaterialType
{
  kDiffuse,
  kReflection,
  kReflectionAndRefraction
};

struct Material
{
  Vec3f color;
  Vec3f::type transparency;
  Vec3f::type reflection;
  float ior    = 1.f;
  Vec3f albedo = 0.18f;
};

#endif // !MATERIAL_DEF_H
