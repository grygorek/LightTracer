#ifndef SCENE_RENDER_H
#define SCENE_RENDER_H

#include <memory>
#include <vector>
#include "image.h"
#include "light.h"
#include "object.h"
#include "geometry.h"

/// Type definiton of objects that make the scene
using ObjectsCollection = std::vector<std::unique_ptr<Object>>;
/// Type definition of lights that are present in the scene
using LightsCollection = std::vector<std::unique_ptr<Light>>;

template <class T>
class View
{
public:
  using Point3d = Vec3<T>;

  Point3d eye, dir;
  T imageWidth, imageHeight;
  T invWidth, invHeight;
  T fov;
  T aspectratio;
  T scale;
  Matrix44<T> cameraCoordinates;

  View(const Point3d &eye_, const Point3d &direction, int imageWidth_, int imageHeight_, T fov_)
      : eye{eye_}
      , dir{direction}
      , imageWidth(static_cast<T>(imageWidth_))
      , imageHeight(static_cast<T>(imageHeight_))
      , invWidth{1.f / imageWidth}
      , invHeight{1.f / imageHeight}
      , fov{fov_}
      , aspectratio{imageWidth / imageHeight}
      , scale{tanf(radians(0.5f * fov))}
      , cameraCoordinates{LookAt(eye, dir)}
  {
  }
};

/// Render scene
///
/// Function firstly sets perspective and runs ray traceing algorithm.
/// It renders given objects with given lights. It saves output scene image
/// into the provided Image object
///
/// @param view - view point and direction
/// @param objects - input objects
/// @param lights - inputs lights
/// @param image - output image
void Render(const View<float> &view, const ObjectsCollection &objects, const LightsCollection &lights, Image &image);

/// Convert image coordinates to the view coordinates.
///
/// @code
/// for(int y=0; y<height; y++)
/// for(int x=0; x<width; x++)
/// {
///   auto p = Perspective(view, x, y);
///   auto rayDir = lookup.multiply_direction(p);
///   image[y*width + x] = Trace(eye, rayDir.normalize(), ...);
/// }
/// @endcode
///
/// @param view - scene view
/// @param imageX - image X axis position
/// @param imageY - image Y axis position
/// @returns Vector of cooridnates converted to the scene
inline Vec3f Perspective(const View<float> &view, int imageX, int imageY)
{
  enum Zaxis
  {
    towardsScreen = -1, /// Zaxis is towards the screen (compatible with OpenGL)
    towardsYou    = 1   /// Zaxis is towards you
  };

  float xx = (2 * ((imageX + 0.5f) * view.invWidth) - 1) * view.scale * view.aspectratio;
  float yy = (1 - 2 * ((imageY + 0.5f) * view.invHeight)) * view.scale;
  return {xx, yy, Zaxis::towardsScreen};
}

inline Vec3f InvertPerspective(const View<float> &view, const Vec3f &point)
{
  Vec3f pt{point.x / -point.z, point.y / -point.z, point.z};

  pt.x = (pt.x / (view.scale * view.aspectratio) + 1) * 0.5f * view.imageWidth - 0.5f;
  pt.y = (0.5f - pt.y / (2 * view.scale)) * view.imageHeight - 0.5f;

  return pt;
}

#endif // !SCENE_RENDER_H
