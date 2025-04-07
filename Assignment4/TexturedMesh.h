#ifndef TEXTUREDMESH_H
#define TEXTUREDMESH_H

#include <string>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "MeshLoader.h" // Provides VertexData, TriData, and readPLYFile

class TexturedMesh {
public:
    TexturedMesh(const std::string &plyFile, const std::string &textureFile);
    ~TexturedMesh();

    glm::vec3 getMinBB() const { return meshMin; }
    glm::vec3 getMaxBB() const { return meshMax; }

    // Draws the mesh using the provided 4x4 model-view-projection matrix.
    void draw(const float* mvpMatrix);
    void printBoundingBox(const std::vector<VertexData>& vertices);

private:
    std::vector<VertexData> vertices;
    std::vector<TriData> faces;

    glm::vec3 meshMin, meshMax;  // bounding box for this mesh

    // OpenGL handles
    GLuint vao;
    GLuint vboVertices;
    GLuint eboIndices;
    GLuint textureID;
    GLuint shaderProgram;

    bool loadMesh(const std::string &filename);
    bool loadTexture(const std::string &textureFile);
    bool setupBuffers();
    bool setupShaders();
};

#endif // TEXTUREDMESH_H
