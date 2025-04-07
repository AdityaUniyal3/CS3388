#ifndef MESHLOADER_H
#define MESHLOADER_H

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <limits>

// Structure for storing vertex data.
// We'll store: position (x, y, z), normal (nx, ny, nz), and texture coords (u, v).
struct VertexData {
    float x, y, z;    // Position
    float nx, ny, nz; // Normal vector
    // float r, g, b;  // (remove color, or keep if needed)
    float u, v;       // Texture coordinates

    VertexData()
      : x(0), y(0), z(0),
        nx(0), ny(0), nz(0),
        // r(1), g(1), b(1),
        u(0), v(0) {}
};

struct TriData {
    int v1, v2, v3;
    TriData() : v1(0), v2(0), v3(0) {}
    TriData(int a, int b, int c) : v1(a), v2(b), v3(c) {}
};

// Inline function to read a PLY file with the assumption that
// each line has x, y, z, nx, ny, nz, u, v in that order, and faces are triangles.
inline bool readPLYFile(const std::string &filename, std::vector<VertexData> &vertices,std::vector<TriData> &faces)
{
    std::ifstream file(filename.c_str());
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open PLY file: " << filename << std::endl;
        return false;
    }

    std::string line;
    bool headerEnded = false;
    int vertexCount = 0;
    int faceCount   = 0;

    // We'll keep track of the property names in the order they appear:
    std::vector<std::string> propertyNames;
    // Also track how many properties per vertex line we expect to read:
    int vertexProperties = 0;

    // =============== 1) READ HEADER ===============
    while (std::getline(file, line))
    {
        // If we already found "end_header", break:
        if (headerEnded) break;

        std::istringstream iss(line);
        std::string token;
        iss >> token;

        if (token == "format")
        {
            // e.g. "format ascii 1.0"
            // We can parse it, but let's just skip for now
        }
        else if (token == "element")
        {
            // e.g. "element vertex 168" or "element face 189"
            std::string which;
            int count;
            iss >> which >> count;
            if (which == "vertex")
            {
                vertexCount = count;
            }
            else if (which == "face")
            {
                faceCount = count;
            }
        }
        else if (token == "property")
        {
            // e.g. "property float x"
            // or    "property float nx"
            // or    "property float u"
            // or    "property uchar red" etc.
            // We'll read the next two tokens: type and name
            std::string ptype, pname;
            iss >> ptype >> pname;
            // We only store the *name* (e.g. "x","y","z","nx","ny","nz","u","v","red","green","blue"...)
            // The user can adapt for r/g/b or red/green/blue as needed.
            propertyNames.push_back(pname);
            vertexProperties++;
        }
        else if (token == "end_header")
        {
            headerEnded = true;
            // we can break here, or let the while loop do it
        }
        // else: other header lines (comment, obj_info, etc.) – ignore them
    }

    if (!headerEnded) {
        std::cerr << "Error: PLY header not properly terminated in "
                  << filename << std::endl;
        return false;
    }

    // =============== 2) READ VERTEX DATA ===============
    if (vertexCount <= 0) {
        std::cerr << "Warning: No vertices in " << filename << std::endl;
    }
    vertices.resize(vertexCount);

    // For each vertex line, read exactly vertexProperties floats
    for (int i = 0; i < vertexCount; i++)
    {
        if (!std::getline(file, line)) {
            std::cerr << "Error: Unexpected EOF while reading vertices.\n";
            return false;
        }
        std::istringstream iss(line);

        VertexData v; // default constructed
        // We'll read each float, store in the correct field if recognized:
        for (int p = 0; p < vertexProperties; p++)
        {
            float tmp;
            if (!(iss >> tmp)) {
                std::cerr << "Error: Could not read property #" << p
                          << " for vertex " << i << std::endl;
                return false;
            }
            // propertyNames[p] tells us which property it is
            const std::string &pname = propertyNames[p];
            if      (pname == "x")    v.x  = tmp;
            else if (pname == "y")    v.y  = tmp;
            else if (pname == "z")    v.z  = tmp;
            else if (pname == "nx")   v.nx = tmp;
            else if (pname == "ny")   v.ny = tmp;
            else if (pname == "nz")   v.nz = tmp;
            else if (pname == "u")    v.u  = tmp;
            else if (pname == "v")    v.v  = tmp;
            // If your PLY uses "red green blue" or "r g b", do e.g.:
            //else if (pname == "red"   || pname == "r") v.r = tmp;
            //else if (pname == "green" || pname == "g") v.g = tmp;
            //else if (pname == "blue"  || pname == "b") v.b = tmp;
            // else if (pname == "alpha") ... etc.
            // If there's a property we don't care about, we simply do nothing
            // and skip it.
        }

        vertices[i] = v;
    }

    // =============== 3) READ FACE DATA ===============
    if (faceCount <= 0) {
        std::cerr << "Warning: No faces in " << filename << std::endl;
    }
    faces.resize(faceCount);

    for (int i = 0; i < faceCount; i++) {
        if (!std::getline(file, line)) {
            std::cerr << "Error: Unexpected EOF while reading faces.\n";
            return false;
        }
        std::istringstream iss(line);
        int vertexPerFace;
        if (!(iss >> vertexPerFace)) {
            std::cerr << "Error: Could not read 'vertex count' for face " << i << std::endl;
            return false;
        }
        if (vertexPerFace != 3) {
            std::cerr << "Error: Face " << i << " is not a triangle (vertex count = "
                      << vertexPerFace << ").\n";
            return false;
        }
        int idx1, idx2, idx3;
        if (!(iss >> idx1 >> idx2 >> idx3)) {
            std::cerr << "Error: Could not read indices for face " << i << std::endl;
            return false;
        }
        // basic bounds check
        if (idx1 < 0 || idx1 >= vertexCount ||
            idx2 < 0 || idx2 >= vertexCount ||
            idx3 < 0 || idx3 >= vertexCount) {
            std::cerr << "Error: Face " << i << " has invalid indices ("
                      << idx1 << ", " << idx2 << ", " << idx3 << ") in "
                      << filename << std::endl;
            return false;
        }
        faces[i] = TriData(idx1, idx2, idx3);
    }

    file.close();

    // =============== 4) PRINT RESULTS ===============
    std::cout << "Loaded " << vertexCount << " vertices and " << faceCount
              << " faces from " << filename << ".\n";
    if (!vertices.empty()) {
        std::cout << "First vertex: ("
                  << vertices[0].x << ", "
                  << vertices[0].y << ", "
                  << vertices[0].z << ")\n";
    } else {
        std::cout << "No vertex data.\n";
    }
    return true;
}


#endif // MESHLOADER_H
