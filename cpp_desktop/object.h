#ifndef BASE_GEOMETRY_OBJECT_H
#define BASE_GEOMETRY_OBJECT_H

#include "geometry.h"
#include "material.h"

class Object;

struct IntersectInfo
{
  Object *obj{};
  float hit_distance{kInfinity};
  int32_t triangle_index;
  Vec2f uv;
};

struct Surface
{
  int32_t idx;
  Vec2f uv;
  Vec3f hit_normal;
  Vec2f hit_texture_coordinates;
  const Material *material;
};

class Object
{
public:
  Object() = default;

  Object(const Matrix44f &obj2world) noexcept
      : objectToWorld(obj2world)
      , worldToObject(obj2world.inverse())
  {
  }

  virtual bool Intersect(const Vec3f &ray_org, const Vec3f &ray_dir,
                         IntersectInfo &isecInfo) const = 0;

  virtual Surface SurfaceProperties(const Vec3f &hitPoint,
                    const IntersectInfo &isecInfo) const = 0;

  virtual ~Object() = default;

  Matrix44f objectToWorld, worldToObject;
};

#endif // !BASE_GEOMETRY_OBJECT_H
