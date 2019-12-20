#include "geometry.h"
#include "sphere.h"
#include <algorithm>
#include <array>
#include <fstream>
#include <future>
#include <iostream>
#include <tuple>
#include <vector>

#define _USE_MATH_DEFINES // for C++
#include <cmath>
#include <corecrt_math_defines.h>

#include "render.h"

constexpr int MAX_RAY_DEPTH = 15;

/// Fresnel factor for non metalic object using Snell's law algirithm
///
/// @param raydir - direction of source ray
/// @param hit_normal - normal vector at the hit point
/// @param ior - index of refraction of a surface at the hit point
/// @param ior_environment - indxe of refraction of surrounding material;
///     air is considered as default environemnt (ior = 1)
/// @returns Fresnel factor of a reflection vector (kr); as a consequence
///    the factor for refraction vector is (1-kr)
float FresnelNonMetalic(const Vec3f &raydir, const Vec3f &hit_normal, float ior,
                        float ior_environment = 1.f)
{
  float cosi = clamp(-1.f, 1.f, raydir.dot(hit_normal));
  float etai = ior_environment, etat = ior;
  if (cosi > 0)
  {
    std::swap(etai, etat);
  }
  // Compute sini using Snell's law
  float sint = etai / etat * sqrtf(std::max(0.f, 1 - cosi * cosi));
  // Total internal reflection
  if (sint >= 1)
  {
    return 1; // kr = 1
  }
  else
  {
    float cost = sqrtf(std::max(0.f, 1 - sint * sint));
    cosi       = fabsf(cosi);
    float Rs =
        ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
    float Rp =
        ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
    return (Rs * Rs + Rp * Rp) / 2; // kr
  }
  // As a consequence of the conservation of energy, transmittance is given
  // by:
  // kt = 1 - kr;
}

/// Fresnel factor for non metalic object using Schlick approximation
/// algorithm
///
/// @remark Air is considered as surrounding material (ior = 1)
///
/// @param mediumRefrIdx - index of refracton of material
/// @param ray2hitNormal - dot product of source ray and normal at a hit point
/// @returns Fresnel factor
static float FresnelNonMetalic(float mediumRefrIdx, float ray2hitNormal)
{
  // Schlick approximation of fresnel effect
  // https: // en.wikipedia.org/wiki/Schlick%27s_approximation
  auto airRefrIdx = 1.0f;
  if (ray2hitNormal > 0)
    std::swap(ray2hitNormal, mediumRefrIdx);
  auto r0 = (airRefrIdx - mediumRefrIdx) / (airRefrIdx + mediumRefrIdx);
  return (r0 + (1 - r0) * pow(1 - ray2hitNormal, 5));
}

/// Finds nearest object to the ray origin
/// @param rayorig - ray position
/// @param raydir - ray direction
/// @param objects - list of objects
/// @returns tupple of a pointer to a nearest object and the distance to it
template <class Objects>
static auto FindNearestObject(const Vec3f &rayorig, const Vec3f &raydir,
                              const Objects &objects)
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
bool IsIntersect(const Object &obj, const Vector &ray_origin,
                 const Vector &ray_dir)
{
  IntersectInfo info;
  return obj->Intersect(ray_origin, ray_dir, info);
}

/// Shade diffused material
///
/// Function calculates colour of the surface at the ray hit point.
/// Point will be shaded depending on provided lights, visiblity and
/// angle of light rays. It will calculate shadow color if any other
/// object hides a light.
///
/// @param phit - coordinates of the hit point at surface
/// @param surf - surface parameters at the hit point
/// @param objects - objects that build the scene; they will be
///     used to intersect with shadow rays
/// @param lights - lights at the scene
/// @returns colour at the hit point
Vec3f DiffuseShader(const Vec3f &phit, const Surface &surf,
                    const ObjectsCollection &objects,
                    const LightsCollection &lights)
{
  Vec3f hitColor{};
  for (const auto &light : lights)
  {
    ShadeProperty shade{light->Illuminate(phit)};
    auto inShade{false};
    for (const auto &obj : objects)
    {
      if (IsIntersect(obj, phit, -shade.lightDir))
      {
        inShade = true;
        break;
      }
    }
    if (!inShade)
      hitColor += surf.material->color * surf.material->albedo *
                  shade.lightIntensity *
                  std::max(0.f, surf.hit_normal.dot(-shade.lightDir));
  };
  return hitColor;
}

/// Refraction vector (transmition throught a transparent surface)
///
/// @param raydir - direction of a source vector
/// @param hit_norm - normal vector at the surface hit point
/// @param ior - index of refraction of surface material
/// @param ior_environment - index of refraction of surrounding material
///                     by default it 1.0 for air
/// @returns direction of refraction vector; could be zero if it full
///     internal reflection
Vec3f Refract(const Vec3f &raydir, const Vec3f &hit_norm, float ior,
              float ior_environment = 1.f)
{
  float cosi = clamp(-1.f, 1.f, raydir.dot(hit_norm));
  float etai = ior_environment, etat = ior;
  Vec3f n = hit_norm;
  if (cosi < 0)
  { // outside the object
    cosi = -cosi;
  }
  else
  { // inside the object
    std::swap(etai, etat);
    n = -n;
  }
  float eta = etai / etat;
  float k   = 1 - eta * eta * (1 - cosi * cosi);
  return k < 0 ? 0 : eta * raydir + (eta * cosi - sqrtf(k)) * n;
}

/// Trace light rays
///
/// Ray is given as its origin and direction. Algorithm is looking for
/// intersections of the given ray with provided scene objects. Depending on
/// the object material it:
///   - calculates shading of the diffuse object
///   - calculates reflection ray for reflective surfaces
///   - calculates refraction and reflection rays for transparent objects
/// Function executes recursevly for reflective and transparent rays.
/// Depth of recursion is limited to MAX_RAY_DEPTH global constant.
///
/// @param rayorig - coordinates of the source of the light ray;
///            could be a hit point for reflected ray
/// @param raydir - normalized ray direction vector
/// @param objectes - collection of objects that make the scene
/// @param lights - collection of lights that make the scene
/// @returns colour at the end of the ray; it is either a colour
///     of an hit object or background
Vec3f Trace(const Vec3f &rayorig, const Vec3f &raydir,
            const ObjectsCollection &objects, const LightsCollection &lights,
            int depth) noexcept
{
  Vec3f background{0.9f, 0.9f, 1.f};
  if (depth > MAX_RAY_DEPTH)
    return background;

  const auto nearestObj = FindNearestObject(rayorig, raydir, objects);
  const auto obj        = nearestObj.obj;
  if (!obj)
    return background;

  const auto phit = rayorig + raydir * nearestObj.hit_distance;
  const auto surf = obj->SurfaceProperties(phit, nearestObj);
  auto bias       = 1e-3f * surf.hit_normal;

  Vec3f color{};
  if (surf.material->reflection || surf.material->transparency)
  {
    float k = 1.f;

    auto cosi = raydir.dot(surf.hit_normal);
    bias      = cosi < 0 ? -bias : bias; // inside of the object => reverse sign

    if (surf.material->transparency)
    {
      k = FresnelNonMetalic(raydir, surf.hit_normal, surf.material->ior);
      if (k < 1)
      { // k==1: all internal reflection, no need to calculate refraction
        auto transmDir =
            Refract(raydir, surf.hit_normal, surf.material->ior).normalize();
        color += (1 - k) *
                 Trace(phit + bias, transmDir, objects, lights, depth + 1) *
                 surf.material->transparency;
      }
    }

    auto reflDir = (raydir - 2 * cosi * surf.hit_normal).normalize();
    color += k * Trace(phit + bias, reflDir, objects, lights, depth + 1);
    color = surf.material->color * color;
  }

  return (1 - surf.material->reflection) *
             DiffuseShader(phit + bias, surf, objects, lights) +
         surf.material->reflection * color;
}

/// Generate single image
///
/// @param view - view point and direction
/// @param objects - scene objectes
/// @param lights - scene lights
/// @param image - output image
void Render(const View &view, const ObjectsCollection &objects,
            const LightsCollection &lights, Image &image)
{
  enum Zaxis
  {
    /// Zaxis is towards the screen (compatible with OpenGL)
    towardsScreen = -1,

    /// Zaxis is towards you
    towardsYou = 1
  };

  const float invWidth    = 1.f / image.width;
  const float invHeight   = 1.f / image.height;
  const float fov         = 15.f;
  const float aspectratio = image.width / static_cast<float>(image.height);
  const auto scale        = tanf(radians(0.5f * fov));

  auto lookAt = LookAt(view.eye, view.dir);

  auto trace_piece = [&](int height_begin, int height_end) {
    // Trace rays
    for (auto y{height_begin}, pixel{height_begin * image.width};
         y < height_end; ++y)
    {
      for (auto x{0}; x < image.width; ++x, ++pixel)
      {
        float xx = (2 * ((x + 0.5f) * invWidth) - 1) * scale * aspectratio;
        float yy = (1 - 2 * ((y + 0.5f) * invHeight)) * scale;
        Vec3f raydir(xx, yy, Zaxis::towardsScreen);
        raydir = lookAt.multDirMatrix(raydir);

        image[pixel] = Trace(view.eye, raydir.normalize(), objects, lights, 0);
      }
    }
  };

  // single thread
  // trace_piece(0, image.height);

  // Split tracing in multiple threads
  const int n = std::thread::hardware_concurrency() * 2;
  auto rem    = image.height / n;
  auto last   = image.height - rem * n;
  std::vector<std::future<void>> tasks(n + (last > 0));
  if (last)
    tasks[n] = std::async(trace_piece, image.height - last, image.height);
  for (auto i{0}; i < n; i++)
    tasks[i] = std::async(trace_piece, image.height / n * i,
                          image.height / n * (i + 1));
}
