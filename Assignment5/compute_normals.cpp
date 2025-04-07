#include "compute_normals.h"
#include <vector>


// Function to compute normals for each vertex
std::vector<float> compute_normals(const std::vector<float>& vertices) {
    std::vector<float> normals;

    // Iterate over each triangle (every 3 vertices)
    for (size_t i = 0; i < vertices.size(); i += 9) {
        // Get three vertices of the triangle
        float v1x = vertices[i];
        float v1y = vertices[i + 1];
        float v1z = vertices[i + 2];

        float v2x = vertices[i + 3];
        float v2y = vertices[i + 4];
        float v2z = vertices[i + 5];

        float v3x = vertices[i + 6];
        float v3y = vertices[i + 7];
        float v3z = vertices[i + 8];

        // Calculate the edge vectors of the triangle
        float e1x = v2x - v1x;
        float e1y = v2y - v1y;
        float e1z = v2z - v1z;

        float e2x = v3x - v1x;
        float e2y = v3y - v1y;
        float e2z = v3z - v1z;

        // Calculate the cross product of the edge vectors to get the normal vector
        float nx = e1y * e2z - e1z * e2y;
        float ny = e1z * e2x - e1x * e2z;
        float nz = e1x * e2y - e1y * e2x;

        // Normalize the normal vector
        float length = std::sqrt(nx * nx + ny * ny + nz * nz);
        nx /= length;
        ny /= length;
        nz /= length;

        // Repeat the normal vector three times
        for (int j = 0; j < 3; ++j) {
            normals.push_back(nx);
            normals.push_back(ny);
            normals.push_back(nz);
        }
    }

    return normals;
}