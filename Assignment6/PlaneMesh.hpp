#ifndef PLANEMESH_HPP
#define PLANEMESH_HPP

#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>

class PlaneMesh {
public:
    PlaneMesh(float min, float max, float stepsize);
    void draw(glm::vec3 lightPos, glm::mat4 V, glm::mat4 P);

private:
    void planeMeshQuads(float min, float max, float stepsize);

    std::vector<float> verts;
    std::vector<float> normals;
    std::vector<int> indices;

    GLuint vao, vboVerts, vboNormals, ebo;
    GLuint shaderProgram;

    int numVerts, numIndices;
    float min, max;
    glm::vec4 modelColor;
};

#endif
