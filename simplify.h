#pragma once
#include <cstdint>

bool simplify(const float * positions,
              const size_t positionsStride,
              const uint32_t * inputIndices,
              const uint32_t inputVertexCount,
              const uint32_t inputTriangleCount);
