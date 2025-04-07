#ifndef SHADER_SOURCE_HPP
#define SHADER_SOURCE_HPP

// Vertex shader source code (GLSL version 120)
const char* vertexShaderSource = R"(
#version 120

attribute vec3 aPos;
attribute vec3 aNormal;

// Required uniforms
uniform mat4 MVP;          // Model-View-Projection matrix
uniform mat4 M;            // Model matrix
uniform mat3 normalMatrix; // transpose(inverse(M))
uniform mat4 V;            // View matrix
uniform vec3 LightDir;     // Light direction in world space

// Varyings for the fragment shader
varying vec3 vFragPos;  
varying vec3 vNormal;
varying vec3 vLightDir;

void main()
{
    // Final clip-space position
    gl_Position = MVP * vec4(aPos, 1.0);

    // Compute world-space position and pass it on
    vec4 worldPos = M * vec4(aPos, 1.0);
    vFragPos = worldPos.xyz;

    // Transform the normal into world space
    // (normalMatrix is typically transpose(inverse(M)), so it does that job.)
    vNormal = normalize(normalMatrix * aNormal);

    // Pass the light direction to the fragment shader.
    // If LightDir is already the direction from the fragment to the light (in world space),
    // we can pass it unmodified.
    vLightDir = LightDir;
}
)";

// Fragment shader source code (GLSL version 120)
const char* fragmentShaderSource = R"(
#version 120

// Interpolated from the vertex shader
varying vec3 vFragPos;
varying vec3 vNormal;
varying vec3 vLightDir;

// Other lighting uniforms
uniform vec3 cameraPos;      // Eye position in world space
uniform vec3 modelColor;     // Base/diffuse color
uniform vec3 ambientColor;   // e.g. (0.2,0.2,0.2)
uniform vec3 specularColor;  // e.g. (1.0,1.0,1.0)
uniform float shininess;     // e.g. 64

void main()
{
    // Normalize the normal
    vec3 N = normalize(vNormal);

    // If vLightDir is from the light to the fragment, you might just do:
    //   vec3 L = normalize(vLightDir);
    // If its from the fragment to the light, you might invert it. Adjust as needed.
    // Lets assume we want the direction from the fragment *toward* the light:
    vec3 L = normalize(-vLightDir);

    // Vector from fragment to the camera (used in specular)
    vec3 V = normalize(cameraPos - vFragPos);

    // 1) Ambient
    vec3 ambient = ambientColor * modelColor;

    // 2) Diffuse (Lambert)
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = modelColor * diff;

    // 3) Specular (Phong reflection model)
    //    R = reflect(-L, N) is the reflection of the incoming light
    vec3 R = reflect(-L, N);
    float spec = pow(max(dot(R, V), 0.0), shininess);
    vec3 specular = specularColor * spec;

    // Combine them
    vec3 finalColor = ambient + diffuse + specular;

    gl_FragColor = vec4(finalColor, 1.0);
}
)";

#endif // SHADER_SOURCE_HPP
