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

  virtual bool Intersect(const Vec3f &ray_org, const Vec3f &ray_dir, IntersectInfo &isecInfo) const = 0;

  virtual Surface SurfaceProperties(const Vec3f &hitPoint, const IntersectInfo &isecInfo) const = 0;

  virtual ~Object() = default;

  Matrix44f objectToWorld, worldToObject;
};

/// Finds nearest object to the ray origin
/// @param rayorig - ray position
/// @param raydir - ray direction
/// @param objects - list of objects
/// @returns IntersectInfo object to a nearest object and the distance to it
template <class Objects>
auto FindNearestObject(const Vec3f &rayorig, const Vec3f &raydir, const Objects &objects)
{
  IntersectInfo closest;
  for (const auto &obj : objects)
  {
    IntersectInfo info;
    if (obj->Intersect(rayorig, raydir, info))
    {
      if (info.hit_distance < closest.hit_distance)
      {
        closest     = info;
        closest.obj = obj.get();
      }
    }
  }
  return closest;
}

/// Check if a given ray intersects with a given object
///
/// @param obj - object to check intersection
/// @param ray_origin - source of the ray
/// @param ray_dir - ray direction
/// @retval true - ray intersects with the object
/// @retval false - ray does not intersect with the object
template <class Object, class Vector>
bool IsIntersect(const Object &obj, const Vector &ray_origin, const Vector &ray_dir)
{
  IntersectInfo info;
  return obj->Intersect(ray_origin, ray_dir, info);
}

#endif // !BASE_GEOMETRY_OBJECT_H
