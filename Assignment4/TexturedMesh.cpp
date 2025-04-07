#include "TexturedMesh.h"
#include "LoadBitmap.h"
#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>
#include <glm/glm.hpp> // for glm::vec3
#include "ShaderUtils.h"

// Vertex shader source for GLSL version 120.
const char* vertexShaderSource = R"(
#version 120
attribute vec3 inPos;
attribute vec2 inTexCoord;
uniform mat4 MVP;
varying vec2 TexCoord;
void main()
{
    gl_Position = MVP * vec4(inPos, 1.0);
    TexCoord = inTexCoord;
}
)";

// Fragment shader source for GLSL version 120.
const char* fragmentShaderSource = R"(
#version 120
varying vec2 TexCoord;
uniform sampler2D textureSampler;
void main()
{
    gl_FragColor = texture2D(textureSampler, TexCoord);
}
)";

// const char* vertexShaderSource = R"(
// #version 120
// attribute vec3 aPos;
// void main(){
//     gl_Position = vec4(aPos, 1.0);
// }
// )";

// // Fragment shader source for GLSL version 120.
// const char* fragmentShaderSource = R"(
// #version 120
// void main(){
//     gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0); // red triangle
// }
// )";


TexturedMesh::TexturedMesh(const std::string &plyFile, const std::string &textureFile)
    : vao(0), vboVertices(0), eboIndices(0), textureID(0), shaderProgram(0)
{
    if (!loadMesh(plyFile))
        std::cerr << "Error loading mesh from " << plyFile << std::endl;
    if (!loadTexture(textureFile))
        std::cerr << "Error loading texture from " << textureFile << std::endl;
    if (!setupBuffers())
        std::cerr << "Error setting up buffers." << std::endl;
    if (!setupShaders())
        std::cerr << "Error setting up shaders." << std::endl;
}

TexturedMesh::~TexturedMesh() {
    glDeleteBuffers(1, &vboVertices);
    glDeleteBuffers(1, &eboIndices);
    glDeleteVertexArrays(1, &vao);
    glDeleteTextures(1, &textureID);
    glDeleteProgram(shaderProgram);
}

bool TexturedMesh::loadMesh(const std::string &filename) {
    bool success = readPLYFile(filename, vertices, faces);
    if (success) {
        printBoundingBox(vertices);
    }
    std::cout << "Mesh " << filename<< " => " << faces.size() << " faces, "<< vertices.size() << " verts." << std::endl;
    return success;
}

void TexturedMesh::printBoundingBox(const std::vector<VertexData>& vertices) {
    if(vertices.empty()) {
        std::cout << "No vertices to compute bounding box." << std::endl;
        return;
    }
    glm::vec3 minVal( std::numeric_limits<float>::max() );
    glm::vec3 maxVal( -std::numeric_limits<float>::max() );

    for(const auto &v : vertices) {
        minVal.x = std::min(minVal.x, v.x);
        minVal.y = std::min(minVal.y, v.y);
        minVal.z = std::min(minVal.z, v.z);
        maxVal.x = std::max(maxVal.x, v.x);
        maxVal.y = std::max(maxVal.y, v.y);
        maxVal.z = std::max(maxVal.z, v.z);
    }

    // Store in the class fields
    meshMin = minVal;
    meshMax = maxVal;

    std::cout << "\nBounding Box:" << std::endl;
    std::cout << "Min: (" << minVal.x << ", " << minVal.y << ", " << minVal.z << ")" << std::endl;
    std::cout << "Max: (" << maxVal.x << ", " << maxVal.y << ", " << maxVal.z << ")\n" << std::endl;
}

bool TexturedMesh::loadTexture(const std::string &textureFile) {
    unsigned char* data = nullptr;
    unsigned int width, height;
    loadARGB_BMP(textureFile.c_str(), &data, &width, &height);
    if (!data)
        return false;
    
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Assuming the image data is in ARGB format, here we use GL_RGBA.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    delete[] data;
    return true;
}

bool TexturedMesh::setupBuffers() {
    // Create a combined array with position (3 floats) and tex coords (2 floats)
    std::vector<float> bufferData;
    for (const auto &v : vertices) {
        bufferData.push_back(v.x);
        bufferData.push_back(v.y);
        bufferData.push_back(v.z);
        bufferData.push_back(v.u);
        bufferData.push_back(v.v);
    }
    
    // Create an index array from face data
    std::vector<GLuint> indices;
    for (const auto &f : faces) {
        indices.push_back(f.v1);
        indices.push_back(f.v2);
        indices.push_back(f.v3);
    }
    
    // Generate and bind VAO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    // Generate VBO and upload vertex data
    glGenBuffers(1, &vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
    glBufferData(GL_ARRAY_BUFFER, bufferData.size() * sizeof(float), bufferData.data(), GL_STATIC_DRAW);
    
    // In GLSL 120, attributes are not automatically bound to locations.
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,              // location = 0
        3, GL_FLOAT, GL_FALSE,
        5*sizeof(float),
        (void*)0        // offset = 0
    );

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,              // location = 1
        2, GL_FLOAT, GL_FALSE,
        5*sizeof(float),
        (void*)(3*sizeof(float)) // offset = 3 floats
    );
    
    // Generate EBO and upload index data
    glGenBuffers(1, &eboIndices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
    
    // Unbind VAO (the EBO remains bound to the VAO)
    glBindVertexArray(0);
    return true;
}

bool TexturedMesh::setupShaders() {
    GLuint vs = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    
    shaderProgram = glCreateProgram();
    // Attach shaders before binding attribute locations
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    
    // Bind attribute locations manually for GLSL 120.
    glBindAttribLocation(shaderProgram, 0, "inPos");
    glBindAttribLocation(shaderProgram, 1, "inTexCoord");
    
    glLinkProgram(shaderProgram);
    
    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Shader Program Linking Error:\n" << infoLog << std::endl;
        return false;
    }
    glDeleteShader(vs);
    glDeleteShader(fs);
    return true;
}

void TexturedMesh::draw(const float* mvpMatrix) {
    glUseProgram(shaderProgram);
    
    GLint mvpLoc = glGetUniformLocation(shaderProgram, "MVP");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, mvpMatrix);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    GLint samplerLoc = glGetUniformLocation(shaderProgram, "textureSampler");
    glUniform1i(samplerLoc, 0);
    
    glBindVertexArray(vao);
    //std::cout << "Drawing mesh with " << faces.size() << " faces, so " << faces.size()*3 << " indices.\n";
    glDrawElements(GL_TRIANGLES, faces.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
    glUseProgram(0);
}
