#ifndef SCENE_RENDER_H
#define SCENE_RENDER_H

#include "image.h"
#include "object.h"
#include <vector>
#include <memory>

using ObjectsCollection = std::vector<std::unique_ptr<ObjectSimple>>;
using LightsCollection = std::vector<std::unique_ptr<ObjectSimple>>;

void render(const ObjectsCollection &objects,
            const ObjectsCollection &lights, Image &image);

#endif // !SCENE_RENDER_H
