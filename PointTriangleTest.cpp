#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <fstream>


void nearestPointOnTriangle(float(&Q)[3], const float(&A)[3], const float(&B)[3], const float(&C)[3], const float(&P)[3])
{
  Q[0] = 0.f;
  Q[1] = 0.f;
  Q[2] = 0.f;
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

