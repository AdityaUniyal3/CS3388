#ifndef MARCHING_CUBES_H
#define MARCHING_CUBES_H

#include <vector>
#include <functional>

// Generates a triangle mesh using the marching cubes algorithm.
// f: scalar field function f(x,y,z)
// isovalue: the isosurface value (boundary)
// min, max: grid boundaries (cube)
// stepsize: size of each grid cell
std::vector<float> marching_cubes(float (*f)(float, float, float), float isovalue, float min, float max, float stepsize);

#endif // MARCHING_CUBES_H
