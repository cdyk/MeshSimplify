#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include "simplify.h"

namespace {



  std::vector<float> positions;
  std::vector<uint32_t> indices;

  bool handleVertexLine(const std::string& path, const unsigned lineNo, std::istringstream& l)
  {
    std::vector<float> v;
    float t;
    while (l >> t) {
      v.push_back(t);
    }
    if (v.size() < 2) {
      std::cerr << path << '@' << lineNo << ": vertex with " << v.size() << " coordinates...?\n";
    }
    if (v.size() < 4) {
      v.resize(4);
      v[3] = 1.f;
    }
    if (4 < v.size()) {
      std::cerr << path << '@' << lineNo << ": vertex with " << v.size() << " coordinates, truncating to 4.\n";
      v.resize(4);
    }
    if (v[3] != 1.f) {
      std::cerr << path << '@' << lineNo << ": non-Euclidean vertex position with non-unit w-coordinate.\n";
    }

    for (unsigned i = 0; i < 3; i++) {
      positions.push_back(v[0]);
    }
    return true;
  }

  bool handleFaceLine(const std::string& path, const unsigned lineNo, std::istringstream& l)
  {
    static bool w1 = false;

    std::string t;
    std::vector<int> ix;
    while (l >> t) {
      if (auto pos = t.find('/'); pos != std::string::npos) {
        t = t.substr(0, pos);
        if (!w1) {
          w1 = true;
          std::cerr << path << '@' << lineNo << ": Mesh has indices to more than vertices, ignoring.\n";
        }
      }
      try {
        ix.push_back(std::stoi(t));
      }
      catch (std::invalid_argument) {
        std::cerr << path << '@' << lineNo << ": Failed to parse unsigned integer, skipping face.\n";
        return false;
      }
    }

    if (ix.size() != 3) {
      std::cerr << path << '@' << lineNo << ": Non-triangle polygon with " << ix.size() << " faces, skipping.\n";
      return false;
    }

    for (unsigned i = 0; i < 3; i++) {
      if (ix[i] < 0) {
        ix[i] = int(positions.size() - ix[i]);
        if (ix[i] < 0) {
          std::cerr << path << '@' << lineNo << ": Negative index " << ix[i] << " points before vertex start, ignoring face.\n";
          return false;
        }
      }
      else if (ix[i] == 0) {
        std::cerr << path << '@' << lineNo << ": Zero index illegal, ignoring face.\n";
        return false;
      }
      else if (positions.size() <= ix[i]) {
        std::cerr << path << '@' << lineNo << ": Index " << ix[i] << " points beyond vertex end, ignoring face.\n";
        return false;
      }
    }
    for (unsigned i = 0; i < 3; i++) {
      indices.push_back(ix[i]);
    }

    return true;
  }

  bool readObjFile(const std::string& path)
  {
    std::ifstream in(path.c_str());
    if (!in.good()) {
      std::cerr << "Failed to open " << path << ".\n";
      return false;
    }

    bool notJustVerticeWarn = false;
    unsigned lineNo = 0;
    std::string line, key;
    while (!in.eof()) {
      lineNo++;
      getline(in, line);
      if (line.empty() || line[0] == '#') continue;

      std::istringstream l(line);
      l >> key;
      if (key == "v") {
        handleVertexLine(path, lineNo, l);
      }
      else if (key == "f") {
        handleFaceLine(path, lineNo, l);
      }
    }

    return true;
  }

}

int main(int argc, char** argv)
{
  if (argc < 2) {
    std::cerr << "Usage " << argv[0] << " <model.obj>\n";
    return 1;
  }
  if (!readObjFile(argv[1])) {
    std::cerr << "Failed to read input model.\n";
    return 1;
  }
  std::cerr << "Read " << positions.size() << " vertices and " << indices.size() << " indices.\n";

  if (!simplify(positions.data(), 3 * sizeof(float), indices.data(), uint32_t(positions.size()), uint32_t(indices.size() / 3))) {
    std::cerr << "Failed to simplify mesh.\n";
    return 1;
  }

  return 0;
}

