#ifndef WRITE_PLY_H
#define WRITE_PLY_H

#include <vector>
#include <string>

// A simple Vertex struct used for PLY file I/O.
struct Vertex {
    float x, y, z;
    float nx, ny, nz;
};

// Write the vertices and normals to a PLY file.
void writePLY(const std::vector<float>& vertices, const std::vector<float>& normals, const std::string& fileName);

// Optionally, a function to read a PLY file (if needed)
void readPLY(const std::string& filename, std::vector<Vertex>& vertices, std::vector<int>& indices);

#endif // WRITE_PLY_H
