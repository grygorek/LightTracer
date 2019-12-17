#include "triangle_mesh.h"
#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>
//
//std::optional<TriangleMesh> LoadPolyMeshFromFile(std::string file,
//                                                 const Matrix44f &o2w)
//{
//  std::ifstream ifs;
//  try
//  {
//    ifs.open(file);
//    if (ifs.fail())
//      return std::nullopt;
//
//    std::stringstream ss;
//    ss << ifs.rdbuf();
//    uint32_t numFaces;
//    ss >> numFaces;
//
//    // reading face index array
//    std::vector<uint32_t> faceIndex(numFaces);
//    uint32_t vertsIndexArraySize = 0;
//    for (uint32_t i = 0; i < numFaces; ++i)
//    {
//      ss >> faceIndex[i];
//      vertsIndexArraySize += faceIndex[i];
//    }
//
//    // reading vertex index array
//    std::vector<uint32_t> vertsIndex(vertsIndexArraySize);
//    uint32_t vertsArraySize = 0;
//    for (uint32_t i = 0; i < vertsIndexArraySize; ++i)
//    {
//      ss >> vertsIndex[i];
//      if (vertsIndex[i] > vertsArraySize)
//        vertsArraySize = vertsIndex[i];
//    }
//    vertsArraySize += 1;
//
//    // reading vertices
//    std::vector<Vec3f> verts(vertsArraySize);
//    for (uint32_t i = 0; i < vertsArraySize; ++i)
//    {
//      ss >> verts[i].x >> verts[i].y >> verts[i].z;
//    }
//
//    // reading normals
//    std::vector<Vec3f> normals(vertsIndexArraySize);
//    for (uint32_t i = 0; i < vertsIndexArraySize; ++i)
//    {
//      ss >> normals[i].x >> normals[i].y >> normals[i].z;
//    }
//
//    // reading st coordinates
//    std::vector<Vec2f> st(vertsIndexArraySize);
//    for (uint32_t i = 0; i < vertsIndexArraySize; ++i)
//    {
//      ss >> st[i].x >> st[i].y;
//    }
//
//    return TriangleMesh(o2w, numFaces, faceIndex, vertsIndex, verts, normals,
//                        st);
//  }
//  catch (...)
//  {
//  }
//
//  return std::nullopt;
//}
