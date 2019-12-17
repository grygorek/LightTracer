#ifndef LIGHTS_H
#define LIGHTS_H

#include "geometry.h"

struct ShadeProperty
{
  Vec3f lightDir;
  Vec3f lightIntensity;
  float lightDistance;
};

/// Light base class
class Light
{
public:
  Light(const Vec3f &color = 1, float intensity = 1)
      : color(color)
      , intensity(intensity)
  {
  }
  virtual ~Light() {}
  virtual ShadeProperty Illuminate(const Vec3f &hitPoint) const = 0;

  Vec3f color;
  float intensity;
};

/// Distant light
class DistantLight : public Light
{
  Vec3f dir;

public:
  DistantLight(const Vec3f &direction, const Vec3f &color = 1,
               float intensity = 1)
      : Light(color, intensity)
      , dir(direction.normalize())
  {
  }

  ShadeProperty Illuminate(const Vec3f &P) const override
  {
    return ShadeProperty{dir, color * intensity, kInfinity};
  }
};

// Point light
class PointLight : public Light
{
  Vec3f position;

public:
  PointLight(const Vec3f &pos, const Vec3f &color = 1, float intensity = 1)
      : Light(color, intensity)
      , position(pos)
  {
  }
  // P: is the shaded point
  ShadeProperty Illuminate(const Vec3f &P) const override
  {
    auto distVec = (P - position);
    auto r2      = distVec.norm();
    auto lIntens = color * intensity * static_cast<float>(1.f / (4.f * M_PI * r2));
    return ShadeProperty{distVec.normalize(), lIntens, sqrt(r2)};
  }
};

#endif // !LIGHTS_H
