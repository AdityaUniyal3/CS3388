#include "PlaneMesh.hpp"
#include "ShaderLoader.hpp"
#include <iostream>
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

PlaneMesh::PlaneMesh(float min, float max, float stepsize) {
    this->min = min;
    this->max = max;
    modelColor = glm::vec4(0, 1.0f, 1.0f, 1.0f);

    planeMeshQuads(min, max, stepsize);
    numVerts = verts.size() / 3;
    numIndices = indices.size();

	//Load shader program here!
    shaderProgram = LoadShaders(
        "shaders/WaterShader.vertexshader",
        "shaders/WaterShader.tcs",
        "shaders/WaterShader.tes",
        "shaders/WaterShader.geoshader",
        "shaders/WaterShader.fragmentshader"
    );

    //shaderProgram = LoadShaders("shaders/debug.vert", "shaders/WaterShader.tcs", "shaders/WaterShader.tes", "", "shaders/debug.frag");

    outerTessLoc = glGetUniformLocation(shaderProgram, "outerTess");
    innerTessLoc = glGetUniformLocation(shaderProgram, "innerTess");

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    unsigned char* data = nullptr;
    unsigned width = 0, height = 0;
    unsigned short bpp = 0;
    loadARGB_BMP("Assets/water.bmp", &data, &width, &height, &bpp);

    glGenTextures(1, &waterTexture);
    glBindTexture(GL_TEXTURE_2D, waterTexture);

    if (data) {
        GLenum format = (bpp == 24) ? GL_RGB : GL_BGRA;
        GLint internalFormat = (bpp == 24) ? GL_RGB : GL_RGBA;
        
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, 
                    format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        delete[] data;
    } else {
        unsigned char fallback[] = {0, 0, 255, 255};
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0,
                    GL_RGBA, GL_UNSIGNED_BYTE, fallback);
    }


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Vertex positions
    glGenBuffers(1, &vboVerts);
    glBindBuffer(GL_ARRAY_BUFFER, vboVerts);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // Normals
    glGenBuffers(1, &vboNormals);
    glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glGenBuffers(1, &vboTexCoords);
    glBindBuffer(GL_ARRAY_BUFFER, vboTexCoords);
    glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(float), texCoords.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // Indices
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void PlaneMesh::planeMeshQuads(float min, float max, float stepsize) {
    texCoords.clear();
    float x = min;
    for (float z = min; z <= max; z += stepsize) {
        verts.push_back(x);
        verts.push_back(0.0f);
        verts.push_back(z);

        // Add UVs (normalized 0-1)
        texCoords.push_back(0.0f);             // U = 0 for first column
        texCoords.push_back((z - min)/(max - min)); // V = 0-1 from min to max

        normals.push_back(0);
        normals.push_back(1);
        normals.push_back(0);
    }

    for (float x = min + stepsize; x <= max; x += stepsize) {
        for (float z = min; z <= max; z += stepsize) {
            verts.push_back(x);
            verts.push_back(0.0f);
            verts.push_back(z);

            // UVs for this vertex
            texCoords.push_back((x - min)/(max - min)); // U = 0-1
            texCoords.push_back((z - min)/(max - min)); // V = 0-1

            normals.push_back(0);
            normals.push_back(1);
            normals.push_back(0);
        }
    }

    int nCols = (max - min) / stepsize + 1;
    int i = 0, j = 0;
    for (float x = min; x < max; x += stepsize) {
        j = 0;
        for (float z = min; z < max; z += stepsize) {
            indices.push_back(i * nCols + j);
            indices.push_back(i * nCols + j + 1);
            indices.push_back((i + 1) * nCols + j + 1);
            indices.push_back((i + 1) * nCols + j);
            ++j;
        }
        ++i;
    }
}

void PlaneMesh::draw(glm::vec3 lightPos, glm::mat4 V, glm::mat4 P) {
    glUseProgram(shaderProgram);

    // Set tessellation levels (verify these exist in your TCS)
    if (outerTessLoc != -1) glUniform1f(outerTessLoc, 16.0f);
    if (innerTessLoc != -1) glUniform1f(innerTessLoc, 16.0f);

    glm::mat4 M = glm::mat4(1.0f);
    glm::mat4 MVP = P * V * M;

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "M"), 1, GL_FALSE, glm::value_ptr(M));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "V"), 1, GL_FALSE, glm::value_ptr(V));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "P"), 1, GL_FALSE, glm::value_ptr(P));

    glUniform1f(glGetUniformLocation(shaderProgram, "time"), (float)glfwGetTime()); 
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, glm::value_ptr(lightPos));

    glm::vec3 eyePos = glm::vec3(glm::inverse(V)[3]); // Extract camera position from view matrix
    glUniform3fv(glGetUniformLocation(shaderProgram, "eyePos"), 1, glm::value_ptr(eyePos));

    // Bind water texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, waterTexture);
    glUniform1i(glGetUniformLocation(shaderProgram, "waterTex"), 0);
    // GLuint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos_worldspace");
    // glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));

    // GLuint timeLoc = glGetUniformLocation(shaderProgram, "time");
    // glUniform1f(timeLoc, (float)glfwGetTime());

    // glBindVertexArray(vao);
    // glPatchParameteri(GL_PATCH_VERTICES, 4);
    // glDrawElements(GL_PATCHES, numIndices, GL_UNSIGNED_INT, 0);
    // glBindVertexArray(0);
    glBindVertexArray(vao);
    //std::cout << "Drawing PlaneMesh with " << numIndices << " indices." << std::endl;
    glPatchParameteri(GL_PATCH_VERTICES, 4);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // Wireframe mode
    glDrawElements(GL_PATCHES, numIndices, GL_UNSIGNED_INT, 0);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // Restore
    glBindVertexArray(0);
}

void PlaneMesh::loadARGB_BMP(const char* imagepath, unsigned char** data, 
                           unsigned int* width, unsigned int* height, unsigned short* bpp) {
    printf("Reading image %s\n", imagepath);
    
    FILE* file = fopen(imagepath, "rb");
    if (!file) {
        printf("%s could not be opened. Are you in the right directory?\n", imagepath);
        *data = nullptr;
        return;
    }

    unsigned char header[54];
    if (fread(header, 1, 54, file) != 54) {
        printf("Not a valid BMP file (header too small)\n");
        fclose(file);
        *data = nullptr;
        return;
    }

    if (header[0] != 'B' || header[1] != 'M') {
        printf("Not a valid BMP file (invalid signature)\n");
        fclose(file);
        *data = nullptr;
        return;
    }

    *width = *(int*)&(header[0x12]);
    *height = *(int*)&(header[0x16]);
    *bpp = *(short*)&(header[0x1C]);

    // Rest of the function remains the same...
    unsigned int dataOffset = *(int*)&(header[0x0A]);
    unsigned int imageSize = *(int*)&(header[0x22]);
    
    if (imageSize == 0) imageSize = (*width) * (*height) * (*bpp/8);
    if (dataOffset == 0) dataOffset = 54;

    *data = new unsigned char[imageSize];
    fseek(file, dataOffset, SEEK_SET);
    fread(*data, 1, imageSize, file);
    fclose(file);

    if (*bpp == 24) {
        for (unsigned int i = 0; i < imageSize; i += 3) {
            std::swap((*data)[i], (*data)[i+2]);
        }
    }
}