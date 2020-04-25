#ifndef TRIANGLE_MESH_H
#define TRIANGLE_MESH_H

#include "geometry.h"
#include "object.h"
#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

inline bool rayTriangleIntersect(const Vec3f &orig, const Vec3f &dir, const Vec3f &v0, const Vec3f &v1, const Vec3f &v2,
                                 float &t, float &u, float &v)
{
  auto v0v1 = v1 - v0;
  auto v0v2 = v2 - v0;
  auto pvec = dir.cross(v0v2);
  auto det  = v0v1.dot(pvec);

  // ray and triangle are parallel if det is close to 0
  if (fabs(det) < kEpsilon)
    return false;

  auto invDet = 1.f / det;

  auto tvec = orig - v0;
  u         = tvec.dot(pvec) * invDet;
  if (u < 0 || u > 1)
    return false;

  auto qvec = tvec.cross(v0v1);
  v         = dir.dot(qvec) * invDet;
  if (v < 0 || u + v > 1)
    return false;

  t = v0v2.dot(qvec) * invDet;

  return (t > 0) ? true : false;
}

class TriangleMesh : public Object
{
public:
  TriangleMesh(TriangleMesh &&mesh) noexcept
  {
    objectToWorld = mesh.objectToWorld;
    worldToObject = mesh.worldToObject;
    numTris       = mesh.numTris;              // number of triangles
    P             = std::move(mesh.P);         // triangles vertex position
    trisIndex     = std::move(mesh.trisIndex); // vertex index array
    N             = std::move(mesh.N);         // triangles vertex normals
    sts           = std::move(mesh.sts);       // triangles texture coordinates
    smoothShading = mesh.smoothShading;        // smooth shading by default
    material      = mesh.material;
  }

  TriangleMesh &operator=(TriangleMesh &&) = delete;
  TriangleMesh operator=(const TriangleMesh &) = delete;

  /// Build a triangle mesh from a face index array and a vertex index array
  TriangleMesh(const Matrix44f &o2w, uint32_t nfaces, const std::vector<uint32_t> &faceIndex,
               const std::vector<uint32_t> &vertsIndex, const std::vector<Vec3f> &verts,
               const std::vector<Vec3f> &normals, const std::vector<Vec2f> &st)
      : Object(o2w)
      , numTris(0)
      , material{Vec3f(.5f), 0, 0}
  {
    uint32_t maxVertIndex = 0;
    // find out how many triangles we need to create for this mesh
    for (uint32_t i = 0, k = 0; i < nfaces; ++i)
    {
      numTris += faceIndex[i] - 2;
      for (uint32_t j = 0; j < faceIndex[i]; ++j)
        if (vertsIndex[k + j] > maxVertIndex)
          maxVertIndex = vertsIndex[k + j];
      k += faceIndex[i];
    }
    maxVertIndex += 1;

    P.resize(maxVertIndex);
    for (uint32_t i = 0; i < maxVertIndex; ++i)
    {
      // Transforming vertices to world space
      P[i] = objectToWorld.multVecMatrix(verts[i]);
    }

    trisIndex.resize(numTris * 3);
    N.resize(numTris * 3);
    sts.resize(numTris * 3);

    // Computing the transpse of the object-to-world inverse matrix
    auto transformNormals = worldToObject.transpose();
    // generate the triangle index array and set normals and st coordinates
    for (uint32_t i = 0, k = 0, l = 0; i < nfaces; ++i)
    {
      // for each  face
      for (uint32_t j = 0; j < faceIndex[i] - 2; ++j)
      {
        // for each triangle in the face
        trisIndex[l]     = vertsIndex[k];
        trisIndex[l + 1] = vertsIndex[k + j + 1];
        trisIndex[l + 2] = vertsIndex[k + j + 2];

        // Transforming normals
        N[l + 0] = transformNormals.multDirMatrix(normals[k]);
        N[l + 1] = transformNormals.multDirMatrix(normals[k + j + 1]);
        N[l + 2] = transformNormals.multDirMatrix(normals[k + j + 2]);

        N[l].normalize();
        N[l + 1].normalize();
        N[l + 2].normalize();
        sts[l]     = st[k];
        sts[l + 1] = st[k + j + 1];
        sts[l + 2] = st[k + j + 2];
        l += 3;
      }
      k += faceIndex[i];
    }
  }

  /// Test if the ray interesests this triangle mesh
  bool Intersect(const Vec3f &orig, const Vec3f &dir, IntersectInfo &isecInfo) const override
  {
    auto j{0};
    bool isect = false;
    for (auto i = 0; i < numTris; ++i)
    {
      const auto &v0 = P[trisIndex[j]];
      const auto &v1 = P[trisIndex[j + 1]];
      const auto &v2 = P[trisIndex[j + 2]];
      float t        = kInfinity, u, v;
      if (rayTriangleIntersect(orig, dir, v0, v1, v2, t, u, v) & (t < isecInfo.hit_distance))
      {
        isecInfo.hit_distance   = t;
        isecInfo.uv             = Vec2f{u, v};
        isecInfo.triangle_index = i;
        isect                   = true;
      }
      j += 3;
    }

    return isect;
  }

  virtual Surface SurfaceProperties(const Vec3f &hitPoint, const IntersectInfo &isecInfo) const override
  {
    Vec3f hitNormal;
    Vec2f hitTextureCoordinates;
    if (smoothShading)
    {
      // vertex normal
      const auto &n0 = N[isecInfo.triangle_index * 3];
      const auto &n1 = N[isecInfo.triangle_index * 3 + 1];
      const auto &n2 = N[isecInfo.triangle_index * 3 + 2];
      hitNormal      = (1 - isecInfo.uv.x - isecInfo.uv.y) * n0 + isecInfo.uv.x * n1 + isecInfo.uv.y * n2;
    }
    else
    {
      // face normal
      const auto &v0 = P[trisIndex[isecInfo.triangle_index * 3]];
      const auto &v1 = P[trisIndex[isecInfo.triangle_index * 3 + 1]];
      const auto &v2 = P[trisIndex[isecInfo.triangle_index * 3 + 2]];
      hitNormal      = (v1 - v0).cross(v2 - v0);
    }

    // doesn't need to be normalized as the N's are normalized but just for
    // safety
    hitNormal.normalize();

    // texture coordinates
    const auto &st0       = sts[isecInfo.triangle_index * 3];
    const auto &st1       = sts[isecInfo.triangle_index * 3 + 1];
    const auto &st2       = sts[isecInfo.triangle_index * 3 + 2];
    hitTextureCoordinates = (1 - isecInfo.uv.x - isecInfo.uv.y) * st0 + isecInfo.uv.x * st1 + isecInfo.uv.y * st2;

    return Surface{isecInfo.triangle_index, 0, hitNormal, hitTextureCoordinates, &material};
  }

  // member variables
  int numTris;                     // number of triangles
  std::vector<Vec3f> P;            // triangles vertex position
  std::vector<uint32_t> trisIndex; // vertex index array
  std::vector<Vec3f> N;            // triangles vertex normals
  std::vector<Vec2f> sts;          // triangles texture coordinates
  bool smoothShading = true;       // smooth shading by default
  Material material;
};

std::unique_ptr<TriangleMesh> LoadPolyMeshFromFile(std::string file, const Matrix44f &o2w);

#endif // !TRIANGLE_MESH_H
