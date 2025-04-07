#version 400
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

// Outputs to TCS (must match TCS inputs!)
out vec3 position_tcs;   // Raw model-space position
out vec3 normal_tcs;     // Model-space normal
out vec2 uv_tcs;         // Texture coordinates
out vec3 eye_tcs;        // Eye vector (world space)
out vec3 light_tcs;      // Light vector (world space)

uniform mat4 M;          // Model matrix
uniform mat4 V;          // View matrix
uniform vec3 lightPos;   // World-space light position
uniform vec3 eyePos;     // World-space camera position

void main() {
    position_tcs = position; // Pass raw model-space position
    normal_tcs = normal;
    uv_tcs = texCoord;

    // Compute world-space position
    vec3 worldPos = vec3(M * vec4(position, 1.0));
    
    // Compute eye and light vectors (world space)
    eye_tcs = eyePos - worldPos;    // Vector from point to camera
    light_tcs = lightPos - worldPos; // Vector from point to light

    // Minimal projection for initial pass-through
    gl_Position = vec4(position, 1.0); // Will be replaced in TES
}