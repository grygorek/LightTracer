#ifndef SCENE_RENDER_H
#define SCENE_RENDER_H

#include "image.h"
#include "light.h"
#include "object.h"
#include <memory>
#include <vector>

/// Type definiton of objects that make the scene
using ObjectsCollection = std::vector<std::unique_ptr<Object>>;
/// Type definition of lights that are present in the scene
using LightsCollection = std::vector<std::unique_ptr<Light>>;

struct View
{
  Vec3f eye, dir;
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
void Render(const View &view, const ObjectsCollection &objects,
            const LightsCollection &lights, Image &image);

#endif // !SCENE_RENDER_H
