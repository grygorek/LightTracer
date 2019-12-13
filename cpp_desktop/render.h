#ifndef SCENE_RENDER_H
#define SCENE_RENDER_H

#include "image.h"
#include "light.h"
#include "object.h"
#include <memory>
#include <vector>

using ObjectsCollection = std::vector<std::unique_ptr<ObjectSimple>>;
using LightsCollection  = std::vector<std::unique_ptr<Light>>;

void render(const ObjectsCollection &objects, const LightsCollection &lights,
            Image &image);

#endif // !SCENE_RENDER_H
