#ifndef SHADERLOADER_HPP
#define SHADERLOADER_HPP

#include <GL/glew.h>

GLuint LoadShaderFromFile(const char* filePath, GLenum shaderType);
GLuint LoadShaders(const char* vertex_file_path, const char* tess_control_file_path, const char* tess_eval_file_path, const char* geometry_file_path, const char* fragment_file_path);

#endif