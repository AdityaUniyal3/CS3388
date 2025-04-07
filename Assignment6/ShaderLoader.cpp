#include "ShaderLoader.hpp"
#include <GL/glew.h>
#include <fstream>
#include <sstream>
#include <iostream>

GLuint LoadShaderFromFile(const char* filePath, GLenum shaderType) {
    std::ifstream shaderFile(filePath);
    if (!shaderFile.is_open()) {
        std::cerr << "Failed to open shader file: " << filePath << std::endl;
        return 0;
    }

    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    std::string shaderStr = shaderStream.str();
    const char* shaderCode = shaderStr.c_str();

    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderCode, NULL);
    glCompileShader(shader);

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        std::cerr << "Shader compilation error in " << filePath << ":\n" << infoLog << std::endl;
    }

    return shader;
}

GLuint LoadShaders(const char* vertex_file_path, const char* tess_control_file_path, const char* tess_eval_file_path, const char* geometry_file_path, const char* fragment_file_path) {

    GLuint ProgramID = glCreateProgram();

    // Declare all shader IDs and initialize to 0
    GLuint VertexShaderID = 0;
    GLuint TessControlID = 0;
    GLuint TessEvalID = 0;
    GLuint GeometryID = 0;
    GLuint FragmentShaderID = 0;

    // Only load and attach if the path is not empty
    if (vertex_file_path && vertex_file_path[0] != '\0') {
        VertexShaderID = LoadShaderFromFile(vertex_file_path, GL_VERTEX_SHADER);
        glAttachShader(ProgramID, VertexShaderID);
    }

    if (tess_control_file_path && tess_control_file_path[0] != '\0') {
        TessControlID = LoadShaderFromFile(tess_control_file_path, GL_TESS_CONTROL_SHADER);
        glAttachShader(ProgramID, TessControlID);
    }

    if (tess_eval_file_path && tess_eval_file_path[0] != '\0') {
        TessEvalID = LoadShaderFromFile(tess_eval_file_path, GL_TESS_EVALUATION_SHADER);
        glAttachShader(ProgramID, TessEvalID);
    }

    if (geometry_file_path && geometry_file_path[0] != '\0') {
        GeometryID = LoadShaderFromFile(geometry_file_path, GL_GEOMETRY_SHADER);
        glAttachShader(ProgramID, GeometryID);
    }

    if (fragment_file_path && fragment_file_path[0] != '\0') {
        FragmentShaderID = LoadShaderFromFile(fragment_file_path, GL_FRAGMENT_SHADER);
        glAttachShader(ProgramID, FragmentShaderID);
    }

    // Link program
    glLinkProgram(ProgramID);

    // Check for link errors
    GLint success;
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetProgramInfoLog(ProgramID, 1024, NULL, infoLog);
        std::cerr << "Shader linking error:\n" << infoLog << std::endl;
    }

    // Cleanup: delete each shader if it was created
    if (VertexShaderID) glDeleteShader(VertexShaderID);
    if (TessControlID)  glDeleteShader(TessControlID);
    if (TessEvalID)     glDeleteShader(TessEvalID);
    if (GeometryID)     glDeleteShader(GeometryID);
    if (FragmentShaderID) glDeleteShader(FragmentShaderID);

    return ProgramID;
}