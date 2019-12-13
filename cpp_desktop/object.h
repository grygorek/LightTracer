#ifndef BASE_GEOMETRY_OBJECT_H
#define BASE_GEOMETRY_OBJECT_H

#include "geometry.h"
#include "material.h"

struct Surface
{
  uint32_t idx;
  Vec2f uv;
  Vec3f hit_normal;
  Vec2f hit_texture_coordinates;
  const Material *material;
  Vec3f albedo = 0.18f;
  float ior    = 1.f;
};

class ObjectSimple
{
public:
  virtual ~ObjectSimple() = default;

  virtual bool intersect(const Vec3f &ray_org, const Vec3f &ray_dir,
                         Vec3f::type &hit_dist, uint32_t &index,
                         Vec2f &uv) const = 0;

  virtual Surface SurfaceProperties(const Vec3f &hitPoint,
                                 const Vec3f &viewDirection) const = 0;
};

class Object
{
public:
  /// Setting up the object-to-world and world-to-object matrix
  Object(const Matrix44f &o2w, const char *name_ = nullptr)
      : objectToWorld(o2w)
      , worldToObject(o2w.inverse())
      , name{name_}
  {
  }

  virtual ~Object() = default;

  virtual bool intersect(const Vec3f &ray_org, const Vec3f &ray_dir,
                         Vec3f::type &hit_dist, uint32_t &index,
                         Vec2f &uv) const = 0;

  virtual void SurfaceProperties(const Vec3f &, const Vec3f &, uint32_t,
                                 const Vec2f &, Vec3f &, Vec2f &) const = 0;

  Matrix44f objectToWorld, worldToObject;
  const char *name;
  MaterialType type = MaterialType::kDiffuse;
  float ior         = 1.f;
  Vec3f albedo      = 0.18f;
};

#endif // !BASE_GEOMETRY_OBJECT_H
