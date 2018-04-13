#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <fstream>
#include <cassert>

//N[0] = u[1]v[2] - u[2]v[1];
//N[1] = u[2]v[0] - u[0]v[2];
//N[2] = u[0]v[1] - u[1]v[0];

void nearestPointOnTriangle(float(&Q)[3], const float(&A)[3], const float(&B)[3], const float(&C)[3], const float(&P)[3])
{
  Q[0] = 0.f;
  Q[1] = 0.f;
  Q[2] = 0.f;


  float PA[3], PB[3], PC[3], AB[3], AC[3], N[3];
  float PAxPB[3];
  float PBxPC[3];
  float PCxPA[3];

  for (unsigned i = 0; i < 3; i++) {
    PA[i] = A[i] - P[i];
    PB[i] = B[i] - P[i];
    PC[i] = C[i] - P[i];

    AB[i] = B[i] - A[i];
    AC[i] = C[i] - A[i];
  }

  // Calc barycentric coords.
  N[0] = AB[1]*AC[2] - AB[2]*AC[1];
  N[1] = AB[2]*AC[0] - AB[0]*AC[2];
  N[2] = AB[0]*AC[1] - AB[1]*AC[0];

  PAxPB[0] = PA[1] * PB[2] - PA[2] * PB[1];
  PAxPB[1] = PA[2] * PB[0] - PA[0] * PB[2];
  PAxPB[2] = PA[0] * PB[1] - PA[1] * PB[0];
  float w_ab = N[0] * PAxPB[0] + N[1] * PAxPB[1] + N[2] * PAxPB[2];

  PBxPC[0] = PB[1] * PC[2] - PB[2] * PC[1];
  PBxPC[1] = PB[2] * PC[0] - PB[0] * PC[2];
  PBxPC[2] = PB[0] * PC[1] - PB[1] * PC[0];
  float w_bc = N[0] * PBxPC[0] + N[1] * PBxPC[1] + N[2] * PBxPC[2];

  PCxPA[0] = PC[1] * PA[2] - PC[2] * PA[1];
  PCxPA[1] = PC[2] * PA[0] - PC[0] * PA[2];
  PCxPA[2] = PC[0] * PA[1] - PC[1] * PA[0];
  float w_ca = N[0] * PCxPA[0] + N[1] * PCxPA[1] + N[2] * PCxPA[2];

  float w = w_ab + w_bc + w_ca;
  assert(0 <= w);

  uint32_t region = (w_ca < 0.f ? 4 : 0) | (w_bc < 0.f ? 2 : 0) | (w_ab < 0.f ? 1 : 0);
  //
  // Region encodes which of the vertices'/edge's Voronoi-region P is in:
  // 
  //       \110=6 |
  //        \CA<0 |
  //         \BC<0|
  //          \   |
  //           \  |
  //            \ |
  //             \|
  //              C
  //          CA<0 |\   BC<0
  //         100=4 | \  010=2
  //               |  \
  //        -------A---B------------
  //          AB<0 |AB<0\ AB<0
  //          CA<0 |     \ BC<0
  //         101=5 | 001=1\ 011=3
  //
  switch (region)
  {
  case 0:   // 000 - inside ab, bc, ca
    break;
  case 1:   // 001 - outside ab; inside bc, ca;
    break;
  case 2:   // 010 - outside bc; inside ab, ca;
    break;
  case 3:   // 011 - outside ab, bc; inside ca;
    break;
  case 4:   // 100 - outside ca; inside bc, ab;
    break;
  case 5:   // 101 - outside ca, ab; inside bc;
    break;
  case 6:   // 110 - outside ca, bc; inside ab
    break;
  case 7:   // 111 - outside ca, bc, ab
    break;
  default:
    break;
  }


}


int main()
{
  const float A[3] = { 0.f, 0.f, 0.f };
  const float B[3] = { 1.f, 0.f, 0.f };
  const float C[3] = { 0.f, 1.f, 0.f };

  const float r = 2.f;
  const unsigned n = 10;
  const unsigned m = 10;


  std::ofstream out("dump.obj");

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
      nearestPointOnTriangle(Q, A, B, C, P);

      out << "v " << Q[0] << ' ' << Q[1] << ' ' << Q[2] << '\n';
      out << "v " << P[0] << ' ' << P[1] << ' ' << P[2] << '\n';
      out << "l -1 -2\n";

    }
  }

  // output triangle
  out << "v " << A[0] << ' ' << A[1] << ' ' << A[2] << '\n';
  out << "v " << B[0] << ' ' << B[1] << ' ' << B[2] << '\n';
  out << "v " << C[0] << ' ' << C[1] << ' ' << C[2] << '\n';
  out << "f -1 -2 -3\n";

  return 0;
}

