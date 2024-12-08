#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <string>
#include "Camera.h"

// Vertex Shader source.
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoord;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
)";

// Fragment Shader source for textures
const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;

// Lighting uniforms
uniform vec3 lightPositions[4]; // Up to 4 lights
uniform vec3 lightColors[4];    // Corresponding colors
uniform vec3 viewPos;

void main() {
    vec3 result = vec3(0.0); // Accumulated light result

    // Ambient lighting
    float ambientStrength = 0.09; // Reduced ambient lighting
    vec3 ambient = vec3(0.0);

    for (int i = 0; i < 4; i++) {
        // Add ambient lighting for this light
        ambient += ambientStrength * lightColors[i];

        // Diffuse lighting
        vec3 norm = vec3(0.0, 1.0, 0.0); // Assuming flat surfaces facing up
        vec3 lightDir = normalize(lightPositions[i] - vec3(0.0, 0.5, 0.0)); // Replace with fragment position if needed
        float diff = max(dot(norm, lightDir), 0.0);

        // Optional: Add attenuation for more realistic lighting
        float distance = length(lightPositions[i] - vec3(0.0, 0.5, 0.0));
        float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));

        vec3 diffuse = diff * lightColors[i] * attenuation * 0.5; // Dim diffuse lighting by 50%

        // Add this light's contribution to the result
        result += ambient + diffuse;
    }

    // Combine lighting result with texture
    FragColor = vec4(result, 1.0) * texture(texture1, TexCoord);
}
)";

// Vertices with texture coordinates
float vertices[] = {
    // Floor (5x larger)
    -6.25f,  0.0f, -6.25f,  0.0f, 0.0f,
     6.25f,  0.0f, -6.25f,  5.0f, 0.0f,
     6.25f,  0.0f,  6.25f,  5.0f, 5.0f,
    -6.25f,  0.0f, -6.25f,  0.0f, 0.0f,
     6.25f,  0.0f,  6.25f,  5.0f, 5.0f,
    -6.25f,  0.0f,  6.25f,  0.0f, 5.0f,

    // ceiling (5x larger)
    -6.25f,  1.0f, -6.25f,  0.0f, 0.0f,
     6.25f,  1.0f, -6.25f,  5.0f, 0.0f,
     6.25f,  1.0f,  6.25f,  5.0f, 5.0f,
    -6.25f,  1.0f, -6.25f,  0.0f, 0.0f,
     6.25f,  1.0f,  6.25f,  5.0f, 5.0f,
    -6.25f,  1.0f,  6.25f,  0.0f, 5.0f,


    // Walls (5 segments per side)
    // Back wall (-Z direction)
    -6.25f, 0.0f, -6.25f,  0.0f, 0.0f,
    -6.25f, 1.0f, -6.25f,  0.0f, 1.0f,
    -3.75f, 1.0f, -6.25f,  1.0f, 1.0f,
    -6.25f, 0.0f, -6.25f,  0.0f, 0.0f,
    -3.75f, 1.0f, -6.25f,  1.0f, 1.0f,
    -3.75f, 0.0f, -6.25f,  1.0f, 0.0f,

    -3.75f, 0.0f, -6.25f,  0.0f, 0.0f,
    -3.75f, 1.0f, -6.25f,  0.0f, 1.0f,
    -1.25f, 1.0f, -6.25f,  1.0f, 1.0f,
    -3.75f, 0.0f, -6.25f,  0.0f, 0.0f,
    -1.25f, 1.0f, -6.25f,  1.0f, 1.0f,
    -1.25f, 0.0f, -6.25f,  1.0f, 0.0f,

    -1.25f, 0.0f, -6.25f,  0.0f, 0.0f,
    -1.25f, 1.0f, -6.25f,  0.0f, 1.0f,
     1.25f, 1.0f, -6.25f,  1.0f, 1.0f,
    -1.25f, 0.0f, -6.25f,  0.0f, 0.0f,
     1.25f, 1.0f, -6.25f,  1.0f, 1.0f,
     1.25f, 0.0f, -6.25f,  1.0f, 0.0f,

     1.25f, 0.0f, -6.25f,  0.0f, 0.0f,
     1.25f, 1.0f, -6.25f,  0.0f, 1.0f,
     3.75f, 1.0f, -6.25f,  1.0f, 1.0f,
     1.25f, 0.0f, -6.25f,  0.0f, 0.0f,
     3.75f, 1.0f, -6.25f,  1.0f, 1.0f,
     3.75f, 0.0f, -6.25f,  1.0f, 0.0f,

     3.75f, 0.0f, -6.25f,  0.0f, 0.0f,
     3.75f, 1.0f, -6.25f,  0.0f, 1.0f,
     6.25f, 1.0f, -6.25f,  1.0f, 1.0f,
     3.75f, 0.0f, -6.25f,  0.0f, 0.0f,
     6.25f, 1.0f, -6.25f,  1.0f, 1.0f,
     6.25f, 0.0f, -6.25f,  1.0f, 0.0f,

     // Front wall (+Z direction)
-6.25f, 0.0f,  6.25f,  0.0f, 0.0f,
-6.25f, 1.0f,  6.25f,  0.0f, 1.0f,
-3.75f, 1.0f,  6.25f,  1.0f, 1.0f,
-6.25f, 0.0f,  6.25f,  0.0f, 0.0f,
-3.75f, 1.0f,  6.25f,  1.0f, 1.0f,
-3.75f, 0.0f,  6.25f,  1.0f, 0.0f,

-3.75f, 0.0f,  6.25f,  0.0f, 0.0f,
-3.75f, 1.0f,  6.25f,  0.0f, 1.0f,
-1.25f, 1.0f,  6.25f,  1.0f, 1.0f,
-3.75f, 0.0f,  6.25f,  0.0f, 0.0f,
-1.25f, 1.0f,  6.25f,  1.0f, 1.0f,
-1.25f, 0.0f,  6.25f,  1.0f, 0.0f,

-1.25f, 0.0f,  6.25f,  0.0f, 0.0f,
-1.25f, 1.0f,  6.25f,  0.0f, 1.0f,
 1.25f, 1.0f,  6.25f,  1.0f, 1.0f,
-1.25f, 0.0f,  6.25f,  0.0f, 0.0f,
 1.25f, 1.0f,  6.25f,  1.0f, 1.0f,
 1.25f, 0.0f,  6.25f,  1.0f, 0.0f,

 1.25f, 0.0f,  6.25f,  0.0f, 0.0f,
 1.25f, 1.0f,  6.25f,  0.0f, 1.0f,
 3.75f, 1.0f,  6.25f,  1.0f, 1.0f,
 1.25f, 0.0f,  6.25f,  0.0f, 0.0f,
 3.75f, 1.0f,  6.25f,  1.0f, 1.0f,
 3.75f, 0.0f,  6.25f,  1.0f, 0.0f,

 3.75f, 0.0f,  6.25f,  0.0f, 0.0f,
 3.75f, 1.0f,  6.25f,  0.0f, 1.0f,
 6.25f, 1.0f,  6.25f,  1.0f, 1.0f,
 3.75f, 0.0f,  6.25f,  0.0f, 0.0f,
 6.25f, 1.0f,  6.25f,  1.0f, 1.0f,
 6.25f, 0.0f,  6.25f,  1.0f, 0.0f,
 // Front wall (+Z direction)
-6.25f, 0.0f,  6.25f,  0.0f, 0.0f,
-6.25f, 1.0f,  6.25f,  0.0f, 1.0f,
-3.75f, 1.0f,  6.25f,  1.0f, 1.0f,
-6.25f, 0.0f,  6.25f,  0.0f, 0.0f,
-3.75f, 1.0f,  6.25f,  1.0f, 1.0f,
-3.75f, 0.0f,  6.25f,  1.0f, 0.0f,

-3.75f, 0.0f,  6.25f,  0.0f, 0.0f,
-3.75f, 1.0f,  6.25f,  0.0f, 1.0f,
-1.25f, 1.0f,  6.25f,  1.0f, 1.0f,
-3.75f, 0.0f,  6.25f,  0.0f, 0.0f,
-1.25f, 1.0f,  6.25f,  1.0f, 1.0f,
-1.25f, 0.0f,  6.25f,  1.0f, 0.0f,

-1.25f, 0.0f,  6.25f,  0.0f, 0.0f,
-1.25f, 1.0f,  6.25f,  0.0f, 1.0f,
1.25f, 1.0f,  6.25f,  1.0f, 1.0f,
-1.25f, 0.0f,  6.25f,  0.0f, 0.0f,
1.25f, 1.0f,  6.25f,  1.0f, 1.0f,
1.25f, 0.0f,  6.25f,  1.0f, 0.0f,

1.25f, 0.0f,  6.25f,  0.0f, 0.0f,
1.25f, 1.0f,  6.25f,  0.0f, 1.0f,
3.75f, 1.0f,  6.25f,  1.0f, 1.0f,
1.25f, 0.0f,  6.25f,  0.0f, 0.0f,
3.75f, 1.0f,  6.25f,  1.0f, 1.0f,
3.75f, 0.0f,  6.25f,  1.0f, 0.0f,

3.75f, 0.0f,  6.25f,  0.0f, 0.0f,
3.75f, 1.0f,  6.25f,  0.0f, 1.0f,
6.25f, 1.0f,  6.25f,  1.0f, 1.0f,
3.75f, 0.0f,  6.25f,  0.0f, 0.0f,
6.25f, 1.0f,  6.25f,  1.0f, 1.0f,
6.25f, 0.0f,  6.25f,  1.0f, 0.0f,

// Left wall (-X direction)
-6.25f, 0.0f, -6.25f,  0.0f, 0.0f,
-6.25f, 1.0f, -6.25f,  0.0f, 1.0f,
-6.25f, 1.0f, -3.75f,  1.0f, 1.0f,
-6.25f, 0.0f, -6.25f,  0.0f, 0.0f,
-6.25f, 1.0f, -3.75f,  1.0f, 1.0f,
-6.25f, 0.0f, -3.75f,  1.0f, 0.0f,

-6.25f, 0.0f, -3.75f,  0.0f, 0.0f,
-6.25f, 1.0f, -3.75f,  0.0f, 1.0f,
-6.25f, 1.0f, -1.25f,  1.0f, 1.0f,
-6.25f, 0.0f, -3.75f,  0.0f, 0.0f,
-6.25f, 1.0f, -1.25f,  1.0f, 1.0f,
-6.25f, 0.0f, -1.25f,  1.0f, 0.0f,

-6.25f, 0.0f, -1.25f,  0.0f, 0.0f,
-6.25f, 1.0f, -1.25f,  0.0f, 1.0f,
-6.25f, 1.0f,  1.25f,  1.0f, 1.0f,
-6.25f, 0.0f, -1.25f,  0.0f, 0.0f,
-6.25f, 1.0f,  1.25f,  1.0f, 1.0f,
-6.25f, 0.0f,  1.25f,  1.0f, 0.0f,

-6.25f, 0.0f,  1.25f,  0.0f, 0.0f,
-6.25f, 1.0f,  1.25f,  0.0f, 1.0f,
-6.25f, 1.0f,  3.75f,  1.0f, 1.0f,
-6.25f, 0.0f,  1.25f,  0.0f, 0.0f,
-6.25f, 1.0f,  3.75f,  1.0f, 1.0f,
-6.25f, 0.0f,  3.75f,  1.0f, 0.0f,

-6.25f, 0.0f,  3.75f,  0.0f, 0.0f,
-6.25f, 1.0f,  3.75f,  0.0f, 1.0f,
-6.25f, 1.0f,  6.25f,  1.0f, 1.0f,
-6.25f, 0.0f,  3.75f,  0.0f, 0.0f,
-6.25f, 1.0f,  6.25f,  1.0f, 1.0f,
-6.25f, 0.0f,  6.25f,  1.0f, 0.0f,
// Left wall (-X direction)
 -6.25f, 0.0f, -6.25f, 0.0f, 0.0f,
 -6.25f, 1.0f, -6.25f, 0.0f, 1.0f,
 -6.25f, 1.0f, -3.75f, 1.0f, 1.0f,
 -6.25f, 0.0f, -6.25f, 0.0f, 0.0f,
 -6.25f, 1.0f, -3.75f, 1.0f, 1.0f,
 -6.25f, 0.0f, -3.75f, 1.0f, 0.0f,

 -6.25f, 0.0f, -3.75f, 0.0f, 0.0f,
 -6.25f, 1.0f, -3.75f, 0.0f, 1.0f,
 -6.25f, 1.0f, -1.25f, 1.0f, 1.0f,
 -6.25f, 0.0f, -3.75f, 0.0f, 0.0f,
 -6.25f, 1.0f, -1.25f, 1.0f, 1.0f,
 -6.25f, 0.0f, -1.25f, 1.0f, 0.0f,

 -6.25f, 0.0f, -1.25f, 0.0f, 0.0f,
 -6.25f, 1.0f, -1.25f, 0.0f, 1.0f,
 -6.25f, 1.0f, 1.25f, 1.0f, 1.0f,
 -6.25f, 0.0f, -1.25f, 0.0f, 0.0f,
 -6.25f, 1.0f, 1.25f, 1.0f, 1.0f,
 -6.25f, 0.0f, 1.25f, 1.0f, 0.0f,

 -6.25f, 0.0f, 1.25f, 0.0f, 0.0f,
 -6.25f, 1.0f, 1.25f, 0.0f, 1.0f,
 -6.25f, 1.0f, 3.75f, 1.0f, 1.0f,
 -6.25f, 0.0f, 1.25f, 0.0f, 0.0f,
 -6.25f, 1.0f, 3.75f, 1.0f, 1.0f,
 -6.25f, 0.0f, 3.75f, 1.0f, 0.0f,

 -6.25f, 0.0f, 3.75f, 0.0f, 0.0f,
 -6.25f, 1.0f, 3.75f, 0.0f, 1.0f,
 -6.25f, 1.0f, 6.25f, 1.0f, 1.0f,
 -6.25f, 0.0f, 3.75f, 0.0f, 0.0f,
 -6.25f, 1.0f, 6.25f, 1.0f, 1.0f,
 -6.25f, 0.0f, 6.25f, 1.0f, 0.0f,

 // Right wall (+X direction)
6.25f, 0.0f, -6.25f,  0.0f, 0.0f,
6.25f, 1.0f, -6.25f,  0.0f, 1.0f,
6.25f, 1.0f, -3.75f,  1.0f, 1.0f,
6.25f, 0.0f, -6.25f,  0.0f, 0.0f,
6.25f, 1.0f, -3.75f,  1.0f, 1.0f,
6.25f, 0.0f, -3.75f,  1.0f, 0.0f,

6.25f, 0.0f, -3.75f,  0.0f, 0.0f,
6.25f, 1.0f, -3.75f,  0.0f, 1.0f,
6.25f, 1.0f, -1.25f,  1.0f, 1.0f,
6.25f, 0.0f, -3.75f,  0.0f, 0.0f,
6.25f, 1.0f, -1.25f,  1.0f, 1.0f,
6.25f, 0.0f, -1.25f,  1.0f, 0.0f,

6.25f, 0.0f, -1.25f,  0.0f, 0.0f,
6.25f, 1.0f, -1.25f,  0.0f, 1.0f,
6.25f, 1.0f,  1.25f,  1.0f, 1.0f,
6.25f, 0.0f, -1.25f,  0.0f, 0.0f,
6.25f, 1.0f,  1.25f,  1.0f, 1.0f,
6.25f, 0.0f,  1.25f,  1.0f, 0.0f,

6.25f, 0.0f,  1.25f,  0.0f, 0.0f,
6.25f, 1.0f,  1.25f,  0.0f, 1.0f,
6.25f, 1.0f,  3.75f,  1.0f, 1.0f,
6.25f, 0.0f,  1.25f,  0.0f, 0.0f,
6.25f, 1.0f,  3.75f,  1.0f, 1.0f,
6.25f, 0.0f,  3.75f,  1.0f, 0.0f,

6.25f, 0.0f,  3.75f,  0.0f, 0.0f,
6.25f, 1.0f,  3.75f,  0.0f, 1.0f,
6.25f, 1.0f,  6.25f,  1.0f, 1.0f,
6.25f, 0.0f,  3.75f,  0.0f, 0.0f,
6.25f, 1.0f,  6.25f,  1.0f, 1.0f,
6.25f, 0.0f,  6.25f,  1.0f, 0.0f,

};

float standVertices[] = {
    // Positions              // Texture Coords
    // Bottom face
    -0.5f, 0.0f, -0.5f,       0.0f, 0.0f,
     0.5f, 0.0f, -0.5f,       1.0f, 0.0f,
     0.5f, 0.0f,  0.5f,       1.0f, 1.0f,
    -0.5f, 0.0f, -0.5f,       0.0f, 0.0f,
     0.5f, 0.0f,  0.5f,       1.0f, 1.0f,
    -0.5f, 0.0f,  0.5f,       0.0f, 1.0f,

    // Top face
    -0.5f, 0.3f, -0.5f,       0.0f, 0.0f,
     0.5f, 0.3f, -0.5f,       1.0f, 0.0f,
     0.5f, 0.3f,  0.5f,       1.0f, 1.0f,
    -0.5f, 0.3f, -0.5f,       0.0f, 0.0f,
     0.5f, 0.3f,  0.5f,       1.0f, 1.0f,
    -0.5f, 0.3f,  0.5f,       0.0f, 1.0f,

    // Front face
    -0.5f, 0.0f,  0.5f,       0.0f, 0.0f,
     0.5f, 0.0f,  0.5f,       1.0f, 0.0f,
     0.5f, 0.3f,  0.5f,       1.0f, 1.0f,
    -0.5f, 0.0f,  0.5f,       0.0f, 0.0f,
     0.5f, 0.3f,  0.5f,       1.0f, 1.0f,
    -0.5f, 0.3f,  0.5f,       0.0f, 1.0f,

    // Back face
    -0.5f, 0.0f, -0.5f,       0.0f, 0.0f,
     0.5f, 0.0f, -0.5f,       1.0f, 0.0f,
     0.5f, 0.3f, -0.5f,       1.0f, 1.0f,
    -0.5f, 0.0f, -0.5f,       0.0f, 0.0f,
     0.5f, 0.3f, -0.5f,       1.0f, 1.0f,
    -0.5f, 0.3f, -0.5f,       0.0f, 1.0f,

    // Left face
    -0.5f, 0.0f, -0.5f,       0.0f, 0.0f,
    -0.5f, 0.0f,  0.5f,       1.0f, 0.0f,
    -0.5f, 0.3f,  0.5f,       1.0f, 1.0f,
    -0.5f, 0.0f, -0.5f,       0.0f, 0.0f,
    -0.5f, 0.3f,  0.5f,       1.0f, 1.0f,
    -0.5f, 0.3f, -0.5f,       0.0f, 1.0f,

    // Right face
     0.5f, 0.0f, -0.5f,       0.0f, 0.0f,
     0.5f, 0.0f,  0.5f,       1.0f, 0.0f,
     0.5f, 0.3f,  0.5f,       1.0f, 1.0f,
     0.5f, 0.0f, -0.5f,       0.0f, 0.0f,
     0.5f, 0.3f,  0.5f,       1.0f, 1.0f,
     0.5f, 0.3f, -0.5f,       0.0f, 1.0f
};

float rectangleVertices[] = {
    // Positions            // Texture Coords
    -0.5f, -0.25f, 0.0f,     0.0f, 0.0f,  // Bottom-left
     0.5f, -0.25f, 0.0f,     1.0f, 0.0f,  // Bottom-right
     0.5f, 0.625f, 0.0f,     1.0f, 1.0f,  // Top-right
    -0.5f, 0.625f, 0.0f,     0.0f, 1.0f   // Top-left
};

// Utility to load textures
unsigned int loadTexture(const char* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else {
        std::cerr << "Failed to load texture: " << path << std::endl;
    }
    stbi_image_free(data);
    return textureID;
}

// Utility to process input
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// Initialize camera

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);

bool keys[1024];
float lastX = 400, lastY = 300;
bool firstMouse = true;
bool rightMouseButtonPressed = false;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (action == GLFW_PRESS)
        keys[key] = true;
    else if (action == GLFW_RELEASE)
        keys[key] = false;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    if (rightMouseButtonPressed) {
        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;
        lastX = xpos;
        lastY = ypos;

        camera.ProcessMouseMovement(xoffset, yoffset);
    } else {
        lastX = xpos;
        lastY = ypos;
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            rightMouseButtonPressed = true;
        } else if (action == GLFW_RELEASE) {
            rightMouseButtonPressed = false;
        }
    }
}


int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Create GLFW window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1920, 1080, "OpenGL mini art gallery", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    float lastFrame = 0.0f;

    // Set callbacks
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // Compile shaders
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Create VAO, VBO
    unsigned int VAO, VBO;
    // Additional VAOs and VBOs for the stand and rectangle
    unsigned int standVAO, standVBO, rectVAO, rectVBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Setup for Stand
    glGenVertexArrays(1, &standVAO);
    glGenBuffers(1, &standVBO);
    glBindVertexArray(standVAO);
    glBindBuffer(GL_ARRAY_BUFFER, standVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(standVertices), standVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Setup for Rectangle
    glGenVertexArrays(1, &rectVAO);
    glGenBuffers(1, &rectVBO);
    glBindVertexArray(rectVAO);
    glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glm::vec3 lightPositions[] = {
        glm::vec3(2.0f, 0.9f, 2.0f),
        glm::vec3(-2.0f, 0.9f, 2.0f),
        glm::vec3(2.0f, 0.9f, -2.0f),
        glm::vec3(-2.0f, 0.9f, -2.0f)
    };

    glm::vec3 lightColors[] = {
        glm::vec3(1.0f, 0.0f, 0.0f), // Red
        glm::vec3(0.0f, 1.0f, 0.0f), // Green
        glm::vec3(0.0f, 0.0f, 0.5f), // Blue
        glm::vec3(1.5f, 1.5f, 1.5f)  // White
    };

    // Load textures
    unsigned int floorTexture = loadTexture("wood-floor-textures.jpg");
    unsigned int wallTexture = loadTexture("white-wall-textures.jpg");
    unsigned int ceilingTexture = loadTexture("ceiling.jpg");

    //Load pics
    unsigned int image1Texture = loadTexture("adam.jpg");
    unsigned int image2Texture = loadTexture("girlwithpearl.jpg");
    unsigned int image3Texture = loadTexture("starry.jpg");
    unsigned int image4Texture = loadTexture("venus.jpg");
    unsigned int image5Texture = loadTexture("mona.jpg");
    unsigned int image6Texture = loadTexture("nightwatch.jpg");
    unsigned int image7Texture = loadTexture("La Grande Jatte.jpg");
    unsigned int image8Texture = loadTexture("lastsupper.jpg");

    unsigned int masterpiece = loadTexture("masterpiece.jpg");
    unsigned int white_gold_marble = loadTexture("white-gold-marble.png");


    // Variables for the rotating camera
    float cameraAngle = 0.0f;  // Angle for rotation in radians
    float cameraSpeed = 0.0002f; // Speed of rotation

    glm::vec3 lightPos(2.0f, 1.0f, 2.0f); // Position of the light
    glm::vec3 lightColor(1.0f, 1.0f, 1.0f); // White light
    glm::vec3 viewPos(0.0f, 0.5f, 0.0f); // Camera/view position

    // Configure shaders
    glUseProgram(shaderProgram);

    // Uniform locations
    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    int viewLoc = glGetUniformLocation(shaderProgram, "view");
    int projLoc = glGetUniformLocation(shaderProgram, "projection");

    // Projection matrix
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1920.0f / 1080.0f, 0.1f, 100.0f);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    int lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
    int lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
    int viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");

    glUseProgram(shaderProgram);
    glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
    glUniform3fv(viewPosLoc, 1, glm::value_ptr(viewPos));

    for (int i = 0; i < 4; i++) {
        std::string lightPosUniform = "lightPositions[" + std::to_string(i) + "]";
        std::string lightColorUniform = "lightColors[" + std::to_string(i) + "]";

        glUniform3fv(glGetUniformLocation(shaderProgram, lightPosUniform.c_str()), 1, glm::value_ptr(lightPositions[i]));
        glUniform3fv(glGetUniformLocation(shaderProgram, lightColorUniform.c_str()), 1, glm::value_ptr(lightColors[i]));
    }

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Rendering loop
        while (!glfwWindowShouldClose(window)) {
            float currentFrame = glfwGetTime();
            float deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            // Process input
            processInput(window);
            camera.ProcessKeyboard(keys, deltaTime);

            // Clear the color and depth buffers
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Use the shader program
            glUseProgram(shaderProgram);

            // Set up view and projection matrices
            glm::mat4 view = camera.GetViewMatrix();
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

            // Render Room
            glBindVertexArray(VAO);
            glm::mat4 model = glm::mat4(1.0f);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            // Render floor
            glBindTexture(GL_TEXTURE_2D, floorTexture);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            // Render ceiling
            glBindTexture(GL_TEXTURE_2D, ceilingTexture);
            glDrawArrays(GL_TRIANGLES, 6, 6);

            // Draw walls
            glBindTexture(GL_TEXTURE_2D, wallTexture);
            glDrawArrays(GL_TRIANGLES, 12, 6);

            glBindTexture(GL_TEXTURE_2D, image1Texture);
            glDrawArrays(GL_TRIANGLES, 18, 6);

            glBindTexture(GL_TEXTURE_2D, wallTexture);
            glDrawArrays(GL_TRIANGLES, 24, 6);

            glBindTexture(GL_TEXTURE_2D, image2Texture);
            glDrawArrays(GL_TRIANGLES, 30, 6);

            glBindTexture(GL_TEXTURE_2D, wallTexture);
            glDrawArrays(GL_TRIANGLES, 36, 6);

            glBindTexture(GL_TEXTURE_2D, image3Texture);
            glDrawArrays(GL_TRIANGLES, 42, 6);

            glBindTexture(GL_TEXTURE_2D, wallTexture);
            glDrawArrays(GL_TRIANGLES, 48, 6);

            glBindTexture(GL_TEXTURE_2D, image4Texture);
            glDrawArrays(GL_TRIANGLES, 54, 6);

            glBindTexture(GL_TEXTURE_2D, wallTexture);
            glDrawArrays(GL_TRIANGLES, 60, 6);

            glBindTexture(GL_TEXTURE_2D, image5Texture);
            glDrawArrays(GL_TRIANGLES, 72, 6);

            glBindTexture(GL_TEXTURE_2D, wallTexture);
            glDrawArrays(GL_TRIANGLES, 78, 6);

            glBindTexture(GL_TEXTURE_2D, image6Texture);
            glDrawArrays(GL_TRIANGLES, 84, 6);

            glBindTexture(GL_TEXTURE_2D, wallTexture);
            glDrawArrays(GL_TRIANGLES, 90, 6);

            glBindTexture(GL_TEXTURE_2D, image7Texture);
            glDrawArrays(GL_TRIANGLES, 96, 6);

            glBindTexture(GL_TEXTURE_2D, wallTexture);
            glDrawArrays(GL_TRIANGLES, 102, 6);

            glBindTexture(GL_TEXTURE_2D, image8Texture);
            glDrawArrays(GL_TRIANGLES, 108, 6);

            glBindTexture(GL_TEXTURE_2D, wallTexture);
            glDrawArrays(GL_TRIANGLES, 114, 6);

            glBindTexture(GL_TEXTURE_2D, wallTexture);
            glDrawArrays(GL_TRIANGLES, 120, 6);

            glBindTexture(GL_TEXTURE_2D, wallTexture);
            glDrawArrays(GL_TRIANGLES, 126, 6);

            glBindTexture(GL_TEXTURE_2D, wallTexture);
            glDrawArrays(GL_TRIANGLES, 132, 30);

            glBindTexture(GL_TEXTURE_2D, wallTexture);
            glDrawArrays(GL_TRIANGLES, 162, 30);

            // Render Stand
            glBindVertexArray(standVAO);
            glBindTexture(GL_TEXTURE_2D, white_gold_marble); // Adjust texture if needed
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // Adjust position as needed
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // Render Rectangle
            glBindVertexArray(rectVAO);
            glBindTexture(GL_TEXTURE_2D, masterpiece); // Use a suitable texture

            // Transform for spinning animation
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0f)); // Position above ground
            float angle = glfwGetTime() * glm::radians(20.0f); // Slow spin (adjust speed if needed)
            model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around Y-axis

            // Pass the model matrix to the shader
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            // Draw the rectangle
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

            // Swap buffers and poll events
            glfwSwapBuffers(window);
            glfwPollEvents();
        }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteTextures(1, &floorTexture);
    glDeleteTextures(1, &wallTexture);

    // Cleanup for stand and rectangle
    glDeleteVertexArrays(1, &standVAO);
    glDeleteBuffers(1, &standVBO);
    glDeleteVertexArrays(1, &rectVAO);
    glDeleteBuffers(1, &rectVBO);

    glfwTerminate();
    return 0;
}
