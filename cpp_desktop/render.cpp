#include "geometry.h"
#include "sphere.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <tuple>
#include <vector>

#define _USE_MATH_DEFINES // for C++
#include <cmath>
#include <corecrt_math_defines.h>

#include "render.h"

#define MAX_RAY_DEPTH 5

static float mix(float a, float b, float mix)
{
  return b * mix + a * (1 - mix);
}

// Evaluate Fresnel equation (ration of reflected light for a given incident
// direction and surface normal)
static float fresnel1(float mediumRefrIdx, float ray2hitNormal)
{
  float kr;
  float cosi = -ray2hitNormal;
  float etai = 1, etat = mediumRefrIdx;
  if (cosi > 0)
  {
    std::swap(etai, etat);
  }
  // Compute sini using Snell's law
  float sint = etai / etat * sqrtf(std::max(0.f, 1 - cosi * cosi));
  // Total internal reflection
  if (sint >= 1)
  {
    kr = 1;
  }
  else
  {
    float cost = sqrtf(std::max(0.f, 1 - sint * sint));
    cosi       = fabsf(cosi);
    float Rs =
        ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
    float Rp =
        ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
    kr = (Rs * Rs + Rp * Rp) / 2;
  }
  return kr;
  // As a consequence of the conservation of energy, transmittance is given by:
  // kt = 1 - kr;
}

static float fresnel(float mediumRefrIdx, float ray2hitNormal)
{
  // Schlick approximation of fresnel effect
  // https: // en.wikipedia.org/wiki/Schlick%27s_approximation
  auto airRefrIdx = 1.0f;
  if (ray2hitNormal > 0)
    std::swap(ray2hitNormal, mediumRefrIdx);
  auto r0 = (airRefrIdx - mediumRefrIdx) / (airRefrIdx + mediumRefrIdx);
  return (r0 + (1 - r0) * pow(1 - ray2hitNormal, 5));
}

/// Finds nearest object to the viewer
/// @param rayorig - viewer position
/// @param raydir - viewing direction
/// @param objects - list of objects
/// @returns tupple of a pointer to a nearest object and the distance to it
template <class Objects>
static auto FindNearestObject(const Vec3f &rayorig, const Vec3f &raydir,
                              const Objects &objects)
{
  const Objects::value_type::element_type *pObj{nullptr};
  float hitNear{INFINITY};
  for (const auto &obj : objects)
  {
    uint32_t i;
    Vec2f uv;
    float t{INFINITY};
    if (obj->intersect(rayorig, raydir, t, i, uv))
    {
      if (t < hitNear)
      {
        hitNear = t;
        pObj    = obj.get();
      }
    }
  }
  return std::make_tuple(pObj, hitNear);
}

 /*Vec3f trace(const Vec3f &rayorig, const Vec3f &raydir,
            const ObjectsCollection &objects, const ObjectsCollection &lights,
            int depth) noexcept
{
  Vec3f background{0.3, 0.3, 0.3};
  if (depth > MAX_RAY_DEPTH)
    return background;

  const auto &nearestObj = FindNearestObject(rayorig, raydir, objects);
  const auto *obj        = std::get<0>(nearestObj);
  if (!obj)
    return background;

  const auto phit = rayorig + raydir * std::get<1>(nearestObj);
  const auto surf = obj->SurfaceProperties(phit, raydir);


}*/

/// Main trace function. It takes a ray as argument (defined by its
/// origin and direction). We test if this ray intersects any of the geometry in
/// the scene. If the ray intersects an object, we compute the intersection
/// point, the normal at the intersection point, and shade this point using this
/// information. Shading depends on the surface property (is it transparent,
/// reflective, diffuse).
///
/// @returns a color for the ray. If the ray intersects an object that is the
/// color of the object at the intersection point, otherwise it returns the
/// background color.
Vec3f trace(const Vec3f &rayorig, const Vec3f &raydir,
            const ObjectsCollection &objects, const ObjectsCollection &lights,
            int depth) noexcept
{
  Vec3f background(.3, .3, .5);
  if (depth > MAX_RAY_DEPTH)
    return background;

  auto nearObj   = FindNearestObject(rayorig, raydir, objects);
  auto nearLight = FindNearestObject(rayorig, raydir, lights);
  if (std::get<1>(nearObj) > std::get<1>(nearLight))
    std::swap(nearObj, nearLight);

  const auto *obj = std::get<0>(nearObj);

  // if there's no intersection return black or background color
  if (!obj)
    return background;

  // point of intersection
  const Vec3f phit = rayorig + raydir * std::get<1>(nearObj);

  auto surface = obj->SurfaceProperties(phit, raydir);

  // color of the ray/surfaceof the object intersected by the ray
  Vec3f surfaceColor{};
  auto bias = 1e-4 * surface.hit_normal;
  if ((surface.material->transparency > 0 || surface.material->reflection > 0))
  {
    // If the normal and the view direction are not opposite to each other
    // reverse the normal direction. That also means we are inside the sphere
    // so
    // set the inside bool to true. Finally reverse the sign of IdotN which
    // we want positive. Add some bias to the point from which we will be
    // tracing.
    bool inside        = false;
    auto ray_dot_nsurf = raydir.dot(surface.hit_normal);
    if (ray_dot_nsurf > 0)
    {
      surface.hit_normal = -surface.hit_normal;
      ray_dot_nsurf      = raydir.dot(surface.hit_normal);
      bias               = -bias;
      inside             = true;
    }

    // compute reflection direction: r  = d - 2(d dot n)n
    Vec3f refldir =
        (raydir - surface.hit_normal * 2 * ray_dot_nsurf).normalize();
    Vec3f reflColour = trace(phit + bias, refldir, objects, lights, depth + 1);

    // if the sphere is also transparent compute refraction ray (transmission)
    Vec3f transmColour{0};
    float refrIdx = 1.1f;
    float eta     = (!inside) ? 1 / refrIdx
                          : refrIdx; // are we inside or outside the surface?
    auto fresnelEffect = fresnel(refrIdx, -ray_dot_nsurf);
    reflColour *= fresnelEffect;
    if (surface.material->transparency)
    {
      float cosi = -ray_dot_nsurf;
      float k    = 1 - eta * eta * (1 - cosi * cosi);
      Vec3f refrdir =
          (raydir * eta + surface.hit_normal * (eta * cosi - sqrt(k)))
              .normalize();
      transmColour = trace(phit - bias, refrdir, objects, lights, depth + 1);
      transmColour *= (1 - fresnelEffect) * surface.material->transparency;
    }
    surfaceColor = (reflColour + transmColour) * surface.material->surfaceColor;
  }
  else
  {
    // it's a diffuse object, no need to raytrace any further
    for (const auto &light : lights)
    {
      auto lightSurf       = light->SurfaceProperties(phit, raydir);
      const auto *lightMat = lightSurf.material;
      bool inShadow        = false;
      auto lightDirection  = -lightSurf.hit_normal;
      for (const auto &object : objects)
      {
        uint32_t idx;
        Vec2f uv;
        float t;
        if (object->intersect(phit + bias, lightDirection, t, idx, uv))
        {
          inShadow = true;
          break;
        }
      }

      // auto hitColor = hitObject->albedo / M_PI * light->intensity *
      // light->color *
      //           std::max(0.f, hitNormal.dotProduct(L));

      if (!inShadow)
        surfaceColor += surface.material->surfaceColor *
                        std::max(0.f, surface.hit_normal.dot(lightDirection)) *
                        lightMat->emissionColor;
    }
  }

  return surfaceColor + surface.material->emissionColor;
}

void render(const ObjectsCollection &objects, const ObjectsCollection &lights,
            Image &image)
{
  const float invWidth    = 1.f / image.width;
  const float invHeight   = 1.f / image.height;
  const float fov         = 30.f;
  const float aspectratio = image.width / static_cast<float>(image.height);
  const float angle       = static_cast<float>(tan(M_PI * 0.5 * fov / 180.));

  // Trace rays
  for (auto y{0}, pixel{0}; y < image.height; ++y)
  {
    for (auto x{0}; x < image.width; ++x, ++pixel)
    {
      float xx = (2 * ((x + 0.5f) * invWidth) - 1) * angle * aspectratio;
      float yy = (1 - 2 * ((y + 0.5f) * invHeight)) * angle;
      Vec3f raydir(xx, yy, -1);
      image[pixel] =
          trace(Vec3f(0, 0, 0), raydir.normalize(), objects, lights, 0);
    }
  }
}
