#include "marching_cubes.h"
#include "TriTable.hpp"  // This should define marching_cubes_lut[256][16]
#include <cmath>
#include <vector>

// Standard table mapping each of the 12 edges to its two corner indices.
static const int edgeEndpoints[12][2] = {
    {0, 1}, {1, 2}, {2, 3}, {3, 0},   // bottom face edges
    {4, 5}, {5, 6}, {6, 7}, {7, 4},   // top face edges
    {0, 4}, {1, 5}, {2, 6}, {3, 7}    // vertical edges
};

std::vector<float> marching_cubes(float (*f)(float, float, float), float isovalue,
                                    float min, float max, float stepsize) {
    std::vector<float> vertices;
    
    // Iterate over the 3D grid
    for (float x = min; x < max; x += stepsize) {
        for (float y = min; y < max; y += stepsize) {
            for (float z = min; z < max; z += stepsize) {

                // Compute scalar field values at the 8 cube corners
                float values[8];
                values[0] = f(x, y, z);
                values[1] = f(x + stepsize, y, z);
                values[2] = f(x + stepsize, y, z + stepsize);
                values[3] = f(x, y, z + stepsize);
                values[4] = f(x, y + stepsize, z);
                values[5] = f(x + stepsize, y + stepsize, z);
                values[6] = f(x + stepsize, y + stepsize, z + stepsize);
                values[7] = f(x, y + stepsize, z + stepsize);

                // Store the 3D positions of each corner
                float cornerPos[8][3] = {
                    {x, y, z},
                    {x + stepsize, y, z},
                    {x + stepsize, y, z + stepsize},
                    {x, y, z + stepsize},
                    {x, y + stepsize, z},
                    {x + stepsize, y + stepsize, z},
                    {x + stepsize, y + stepsize, z + stepsize},
                    {x, y + stepsize, z + stepsize}
                };

                // Determine cube configuration index using bitmasking
                int cubeIndex = 0;
                if (values[0] < isovalue) cubeIndex |= 1;
                if (values[1] < isovalue) cubeIndex |= 2;
                if (values[2] < isovalue) cubeIndex |= 4;
                if (values[3] < isovalue) cubeIndex |= 8;
                if (values[4] < isovalue) cubeIndex |= 16;
                if (values[5] < isovalue) cubeIndex |= 32;
                if (values[6] < isovalue) cubeIndex |= 64;
                if (values[7] < isovalue) cubeIndex |= 128;

                // Get the triangle table for this cube configuration.
                // Assumes marching_cubes_lut is defined such that each entry is a list of
                // edge indices (0..11) terminated with -1.
                int* edgeList = marching_cubes_lut[cubeIndex];
                if (edgeList[0] == -1)
                    continue;  // No intersections in this cube

                // For each triangle defined by the table (3 edges per triangle)
                for (int i = 0; edgeList[i] != -1; i += 3) {
                    for (int j = 0; j < 3; j++) {
                        int edgeIndex = edgeList[i + j]; // an edge index (0..11)
                        // Get the two endpoints (corner indices) of this edge
                        int c1 = edgeEndpoints[edgeIndex][0];
                        int c2 = edgeEndpoints[edgeIndex][1];
                        
                        // Compute the interpolation factor alpha.
                        // Guard against division by zero.
                        float f1 = values[c1];
                        float f2 = values[c2];
                        float alpha = 0.5f; // default to midpoint if f2 == f1
                        if (fabs(f2 - f1) > 1e-6)
                            alpha = (isovalue - f1) / (f2 - f1);
                        
                        // Interpolate between the two corner positions:
                        float vx = cornerPos[c1][0] + alpha * (cornerPos[c2][0] - cornerPos[c1][0]);
                        float vy = cornerPos[c1][1] + alpha * (cornerPos[c2][1] - cornerPos[c1][1]);
                        float vz = cornerPos[c1][2] + alpha * (cornerPos[c2][2] - cornerPos[c1][2]);
                        
                        // Add the interpolated vertex to the vertex list
                        vertices.push_back(vx);
                        vertices.push_back(vy);
                        vertices.push_back(vz);
                    }
                }
            }
        }
    }
    return vertices;
}
