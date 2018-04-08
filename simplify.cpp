#include <cassert>
#include <vector>
#include <algorithm>
#include <iostream>

#include "simplify.h"

namespace {

  struct Vertex
  {
    uint32_t leading;
  };

  struct HalfEdge
  {
    uint32_t to;    // The vertex the half-edge points to.
    uint32_t next;  // The next half-edge 
    uint32_t twin;
    uint32_t face;
  };



}

void populateHalfEdges(std::vector<Vertex>& vertices, std::vector<HalfEdge>& halfEdges, const uint32_t* indices, const uint32_t Nv, const uint32_t Nt)
{
  vertices.clear();
  vertices.resize(Nv, { ~0u });

  halfEdges.clear();
  halfEdges.resize(3 * Nt);

  for (uint32_t t = 0; t < Nt; t++) {
    auto tt = 3 * t;
    for (uint32_t i = 0; i < 3; i++) {

      const auto a = tt + i;
      const auto b = tt + std::min(i - 2u, i + 1u);
      halfEdges[a].to = indices[b];
      halfEdges[a].next = b;
      halfEdges[a].twin = ~0u;
      halfEdges[a].face = t;

      vertices[indices[a]].leading = a;
    }
  }
}

void connectHalfEdges(std::vector<HalfEdge>& halfEdges, const uint32_t* indices, const uint32_t Nv, const uint32_t Nt)
{
  struct SortHelper {
    uint32_t major;
    union {
      uint32_t next;
      uint32_t he;
    };
  };

  std::vector<uint32_t> head(Nv, ~0u);
  std::vector<SortHelper> sortHelper(3 * Nt);

  for (uint32_t t = 0; t < Nt; t++) {
    for (uint32_t i = 0; i < 3; i++) {
      const auto a = 3 * t + i;
      const auto b = 3 * t + std::min(i - 2u, i + 1u);
      const auto vMin = std::min(indices[a], indices[b]);
      const auto vMaj = std::max(indices[a], indices[b]);
      sortHelper[a].major = vMaj;
      sortHelper[a].next = head[vMin];
      head[vMin] = a;
    }
  }

  unsigned boundaryEdges = 0;
  unsigned manifoldEdges = 0;
  unsigned nonManinfoldEdges = 0;

  std::vector<SortHelper> sub;
  sub.reserve(3 * Nt);
  for (uint32_t v = 0; v < Nv; v++) {
    sub.clear();
    for (auto he = head[v]; he != ~0u; he = sortHelper[he].next) {
      sub.push_back(SortHelper{ sortHelper[he].major, he });
    }
    std::sort(sub.begin(), sub.end(), [](auto a, auto b) { return a.major < b.major; });

    for (uint32_t j = 0; j + 1 < sub.size(); ) {
      const auto maj = sub[j].major;
      uint32_t i;
      for (i = j + 1; i < sub.size() && sub[i].major == maj; i++);
      auto k = i - j;
      if (k == 1) { boundaryEdges++; }
      else if (k == 2) {
        halfEdges[sub[j].he].twin = sub[j + 1].he;
        halfEdges[sub[j + 1].he].twin = sub[j].he;
        manifoldEdges++;
      }
      else { nonManinfoldEdges++; }
      j = i;
    }
  }

  std::cerr << boundaryEdges << " boundary edges, " << manifoldEdges << " manifold edges, and " << nonManinfoldEdges << " non-manifold edges.\n";
}


bool simplify(const float * positions,
              const size_t positionsStride,
              const uint32_t * inputIndices,
              const uint32_t inputVertexCount,
              const uint32_t inputTriangleCount)
{
  std::vector<Vertex> vertices;
  std::vector<HalfEdge> halfEdges;

  populateHalfEdges(vertices, halfEdges, inputIndices, inputVertexCount, inputTriangleCount);
  connectHalfEdges(halfEdges, inputIndices, inputVertexCount, inputTriangleCount);



  return true;
}
