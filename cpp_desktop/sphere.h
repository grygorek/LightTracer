#ifndef SPHERE_H
#define SPHERE_H

#include "geometry.h"
#include "object.h"
#include <cmath>

template <class T>
inline bool solveQuadratic(T a, T b, T c, T &x0, T &x1)
{
  T discr = b * b - 4 * a * c;
  if (discr < 0)
    return false;

  if (discr < 1e-10f)
  {
    x0 = x1 = (-0.5 * b / a);
    return true;
  }

  T q = (b > 0) ? (-0.5 * (b + sqrt(discr))) : (-0.5 * (b - sqrt(discr)));
  x0  = q / a;
  x1  = c / q;

  return true;
}

class Sphere : public Object
{
public:
  Vec3f center;                /// position of the sphere
  Vec3f::type radius, radius2; /// sphere radius and radius^2
  Material material;
  Sphere(const Vec3f &center_, float radius_, const Vec3f &surfaceColor_,
         Vec3f::type reflection_ = 0, Vec3f::type transparency_ = 0) noexcept
      : center(center_)
      , radius(radius_)
      , radius2(radius * radius)
      , material{surfaceColor_, transparency_, reflection_,
                 transparency_ > 0 ? 1.5f : 1.f}
  { /* empty */
  }
#if 0
  bool intersect(const Vec3f &ray_org, const Vec3f &ray_dir,
                 Vec3f::type &hit_dist, uint32_t &index,
                 Vec2f &uv) const noexcept override
  // uint32_t &triIndex, // not used for sphere
  // Vec2f &uv) const    // not used for sphere
  {
    // analytic solution
    Vec3f L = orig - center;
    float a = dir.dot(dir);
    float b = 2 * dir.dot(L);
    float c = L.dot(L) - radius2;
    float t0, t1; // solutions for t if the ray intersects
    if (!solveQuadratic(a, b, c, t0, t1))
      return false;

    if (t0 > t1)
      std::swap(t0, t1);

    if (t0 < 0)
    {
      t0 = t1; // if t0 is negative, let's use t1 instead
      if (t0 < 0)
        return false; // both t0 and t1 are negative
    }

    tNear = t0;

    return true;
  }
#elif 1
  bool Intersect(const Vec3f &ray_orig, const Vec3f &ray_dir,
                 IntersectInfo &isecInfo) const noexcept override
  {
    // Compute a ray - sphere intersection using the geometric solution
    // https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection

    Vec3f l  = center - ray_orig;
    auto tca = l.dot(ray_dir);
    // Is the sphere near the viewing direction?
    if (tca < 0)
      return false;

    // It is on the path, do we hit it?
    auto d2 = l.dot(l) - tca * tca;
    if (d2 > radius2)
      return false;

    // We do hit it, calculate intersection distance.
    auto thc = sqrtf(radius2 - d2);
    auto t0  = tca - thc;
    auto t1  = tca + thc;

    // if t0>0 and t1>0 -> whole sphere is in front of us
    // if t0<0 and t1>0 -> we are inside the sphere
    // if t0<0 and t1<0 -> whole sphere is behind us
    // if (t0 > t1)
    //  std::swap(t0, t1);
    if (t1 < 0)
      return false;

    isecInfo.hit_distance = t0 < 0 ? t1 : t0;

    return true;
  }
#endif

  // Set surface data such as normal and texture coordinates at a given point on
  // the surface
  Surface SurfaceProperties(const Vec3f &hitPoint,
                            const IntersectInfo &isecInfo) const override
  {
    auto hitNormal = (hitPoint - center).normalize();

    // In this particular case, the normal is simular to a point on a unit
    // sphere centred around the origin. We can thus use the normal coordinates
    // to compute the spherical coordinates of Phit. atan2 returns a value in
    // the range [-pi, pi] and we need to remap it to range [0, 1] acosf returns
    // a value in the range [0, pi] and we also need to remap it to the range
    // [0, 1]
    return Surface{
        0,
        0,
        hitNormal,
        {static_cast<typename Vec3f::type>(
             (1 + atan2(hitNormal.z, hitNormal.x) / M_PI) * 0.5),
         static_cast<typename Vec3f::type>(acosf(hitNormal.y) / M_PI)},
        &material};
  }
};

#endif
