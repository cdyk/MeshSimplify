#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <fstream>
#include <cassert>
#include <string>

//N[0] = u[1]v[2] - u[2]v[1];
//N[1] = u[2]v[0] - u[0]v[2];
//N[2] = u[0]v[1] - u[1]v[0];

unsigned nearestPointOnTriangle(float(&Q)[3], const float(&A)[3], const float(&B)[3], const float(&C)[3], const float(&P)[3])
{
  // Algorithm:
  // - For each edge, form an outwards plane NxAB, NxBC, and NxCA.
  // - Distance from these planes, e.g. <AP,NxAB> forms barycentric coords when they are normalized.
  // - These half-planes also forms the boundary of the Voronoi-cells of the vertices and edges of the triangle.
  // - By inspecting the sign of the plane distances, we can determine which region P is in:
  // 
  //        \110=6 |
  //         \CA<0 |
  //          \BC<0|
  //           \   |
  //            \  |
  //             \ |
  //              \|
  //               C
  //          CA<0 |\   BC<0
  //         100=4 | \  010=2
  //               |  \
  //        -------A---B------------
  //          AB<0 |AB<0\ AB<0
  //          CA<0 |     \ BC<0
  //         101=5 | 001=1\ 011=3
  //
  // - If all distances are positive, the closest point is inside the triangle,
  //   and we use the barycentric coords to find the position.
  // - If one distance is positive and two negative, the closest point is a
  //   corner, and we use that position.
  // - If two distances are positive and one negative, the closest point is on
  //   an edge. If on edge AB, the ratio of <AP,AB> and <PB,AB> (P orthognoally
  //   projected onto AB) gives the position.

  float AP[3];
  float BP[3];
  float CP[3];
  float N[3];
  float AB[3];
  float BC[3];
  float CA[3];
  for (unsigned i = 0; i < 3; i++) {
    AP[i] = P[i] - A[i];
    BP[i] = P[i] - B[i];
    CP[i] = P[i] - C[i];

    AB[i] = B[i] - A[i];
    BC[i] = C[i] - B[i];
    CA[i] = A[i] - C[i];
  }

  // Calc barycentric coords.
  N[0] = AB[1] * BC[2] - AB[2] * BC[1];
  N[1] = AB[2] * BC[0] - AB[0] * BC[2];
  N[2] = AB[0] * BC[1] - AB[1] * BC[0];

  float NxAB[3];
  NxAB[0] = N[1] * AB[2] - N[2] * AB[1];
  NxAB[1] = N[2] * AB[0] - N[0] * AB[2];
  NxAB[2] = N[0] * AB[1] - N[1] * AB[0];
  float AP_dot_NxAB =
    AP[0] * NxAB[0] +
    AP[1] * NxAB[1] +
    AP[2] * NxAB[2];

  float NxBC[3];
  NxBC[0] = N[1] * BC[2] - N[2] * BC[1];
  NxBC[1] = N[2] * BC[0] - N[0] * BC[2];
  NxBC[2] = N[0] * BC[1] - N[1] * BC[0];
  float BP_dot_NxBC =
    BP[0] * NxBC[0] +
    BP[1] * NxBC[1] +
    BP[2] * NxBC[2];

  float NxCA[3];
  NxCA[0] = N[1] * CA[2] - N[2] * CA[1];
  NxCA[1] = N[2] * CA[0] - N[0] * CA[2];
  NxCA[2] = N[0] * CA[1] - N[1] * CA[0];
  float CP_dot_NxCA =
    CP[0] * NxCA[0] +
    CP[1] * NxCA[1] +
    CP[2] * NxCA[2];


  uint32_t region = (CP_dot_NxCA < 0.f ? 4 : 0) | (BP_dot_NxBC < 0.f ? 2 : 0) | (AP_dot_NxAB < 0.f ? 1 : 0);
  float w_a = 0.f;
  float w_b = 0.f;
  float w_c = 0.f;

  float* e0 = nullptr;
  float* e1 = nullptr;
  float* e2 = nullptr;

  switch (region)
  {
  case 0:   // 000 - inside ab, bc, ca
    w_a = BP_dot_NxBC;
    w_b = CP_dot_NxCA;
    w_c = AP_dot_NxAB;
    break;
   
  case 3:   // 011 - outside ab, bc; inside ca;
    w_b = 1.f;
    break;
  case 5:   // 101 - outside ca, ab; inside bc;
    w_a = 1.f;
    break;
  case 6:   // 110 - outside ca, bc; inside ab
    w_c = 1.f;
    break;

  case 1:   // 001 - outside ab; inside bc, ca;
    w_a = -(BP[0] * AB[0] + BP[1] * AB[1] + BP[2] * AB[2]);
    w_b = AP[0] * AB[0] + AP[1] * AB[1] + AP[2] * AB[2];
    break;
  case 2:   // 010 - outside bc; inside ab, ca;
    w_b = -(CP[0] * BC[0] + CP[1] * BC[1] + CP[2] * BC[2]);
    w_c = BP[0] * BC[0] + BP[1] * BC[1] + BP[2] * BC[2];
    break;
  case 4:   // 100 - outside ca; inside bc, ab;
    e0 = AP;
    w_c = -(AP[0] * CA[0] + AP[1] * CA[1] + AP[2] * CA[2]);
    w_a = CP[0] * CA[0] + CP[1] * CA[1] + CP[2] * CA[2];
    break;

  case 7:   // 111 - outside ca, bc, ab
    w_a = w_b = w_c = 1.f;
    break;
  default:
    break;
  }

  if (e0) {


  }

  w_a = w_a < 0.f ? 0.f : w_a;
  w_b = w_b < 0.f ? 0.f : w_b;
  w_c = w_c < 0.f ? 0.f : w_c;

  float s = 1.f/(w_a + w_b + w_c);
  for (unsigned i = 0; i < 3; i++) {
    Q[i] = s * (w_a * A[i] + w_b * B[i] + w_c * C[i]);
  }

  return region;
}


int main()
{
  const float A[3] = { -0.3f, -0.4f, -0.5f };
  const float B[3] = { 0.9f, 0.9f, 0.1f };
  const float C[3] = { -0.3f, 0.9f, -0.4f };

  const float r = 2.f;
  const unsigned n = 40;
  const unsigned m = 20;


  std::ofstream mat("dump.mtl");
  for (unsigned i = 0; i < 8; i++) {
    mat << "newmtl m" << i << "\n";
    mat << "Kd "
      << (i & 1 ? 1 : 0) << ' '
      << (i & 2 ? 1 : 0) << ' '
      << (i & 4 ? 1 : 0) << '\n';
  }

  std::ofstream out("dump.obj");
  out << "mtllib dump.mtl\n";

  for (unsigned j = 0; j < m; j++) {
    for (unsigned i = 0; i < n; i++) {
      auto theta = (float(M_PI)*j) / m;
      auto phi = (float(2.0*M_PI)*i) / n;

      const float P[3] = {
        r * std::sin(theta)*std::cos(phi),
        r * std::sin(theta)*std::sin(phi),
        r * std::cos(theta)
      };
      float Q[3];
      auto region = nearestPointOnTriangle(Q, A, B, C, P);
      out << "usemtl m" << ((region + 1) & 7) << "\n";

      out << "v " << Q[0] << ' ' << Q[1] << ' ' << Q[2] << '\n';
      out << "v " << P[0] << ' ' << P[1] << ' ' << P[2] << '\n';
      out << "l -1 -2\n";

    }
  }

  // output triangle
  out << "usemtl m0\n";
  out << "v " << A[0] << ' ' << A[1] << ' ' << A[2] << '\n';
  out << "v " << B[0] << ' ' << B[1] << ' ' << B[2] << '\n';
  out << "v " << C[0] << ' ' << C[1] << ' ' << C[2] << '\n';
  out << "f -1 -2 -3\n";

  return 0;
}

