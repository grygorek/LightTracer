#ifndef LIGHTS_H
#define LIGHTS_H

#include "geometry.h"

/// Light base class
class Light
{
public:
  Light(const Vec3f &pos, const Vec3f &c = 1, const float &i = 1)
      : color(c)
      , intensity(i)
      , position(pos)
  {
  }
  virtual ~Light() {}
  virtual void Illuminate(const Vec3f &hitPoint, Vec3f &lightDir,
                          Vec3f &lightIntensity,
                          float &lightDistance) const = 0;

  Vec3f color;
  float intensity;
  Vec3f position;
};

/// Distant light
class DistantLight : public Light
{
  Vec3f dir;

public:
  DistantLight(const Vec3f &position, const Vec3f &direction,
               const Vec3f &c = 1, const float &i = 1)
      : Light(position, c, i)
      , dir(direction.normalize())
  {
  }

  void Illuminate(const Vec3f &P, Vec3f &lightDir, Vec3f &lightIntensity,
                  float &distance) const override
  {
    lightDir       = dir;
    lightIntensity = color * intensity;
    distance       = kInfinity;
  }
};

// Point light
class PointLight : public Light
{

public:
  PointLight(const Vec3f &pos, const Vec3f &c = 1, const float &i = 1)
      : Light(pos, c, i)
  {
  }
  // P: is the shaded point
  void Illuminate(const Vec3f &P, Vec3f &lightDir, Vec3f &lightIntensity,
                  float &distance) const
  {
    lightDir = (P - position);
    float r2 = lightDir.norm();
    distance = sqrt(r2);
    lightDir = lightDir / distance;
    // avoid division by 0
    lightIntensity = color * intensity * (1.f / (4 * M_PI * r2));
  }
};

#endif // !LIGHTS_H
