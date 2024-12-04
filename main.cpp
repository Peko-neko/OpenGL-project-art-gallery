#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Vertex Shader source
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

void main() {
    FragColor = texture(texture1, TexCoord);
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

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Textured Room", nullptr, nullptr);
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
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


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

    // Variables for the rotating camera
    float cameraAngle = 0.0f;  // Angle for rotation in radians
    float cameraSpeed = 0.0002f; // Speed of rotation

    // Configure shaders
    glUseProgram(shaderProgram);

    // Uniform locations
    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    int viewLoc = glGetUniformLocation(shaderProgram, "view");
    int projLoc = glGetUniformLocation(shaderProgram, "projection");

    // Projection matrix
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Rendering loop
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Calculate the new look-at direction
        float lookX = sin(cameraAngle);
        float lookZ = cos(cameraAngle);

        glm::mat4 view = glm::lookAt(
            glm::vec3(0.0f, 0.5f, 0.0f),  // Camera position
            glm::vec3(lookX, 0.5f, lookZ),  // Look at point
            glm::vec3(0.0f, 1.0f, 0.0f)   // Up direction
        );

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        cameraAngle += cameraSpeed;

        glm::mat4 model = glm::mat4(1.0f);  // Identity matrix
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        // Draw floor and ceiling
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindTexture(GL_TEXTURE_2D, ceilingTexture);
        glDrawArrays(GL_TRIANGLES, 6, 6);

        // Draw wall 1
        glBindTexture(GL_TEXTURE_2D, wallTexture);
        glDrawArrays(GL_TRIANGLES, 12, 6);

        // Draw wall 2
        glBindTexture(GL_TEXTURE_2D, image1Texture);
        glDrawArrays(GL_TRIANGLES, 18, 6);

        //Draw wall 3
        glBindTexture(GL_TEXTURE_2D, wallTexture);
        glDrawArrays(GL_TRIANGLES, 24, 6);

        //Draw wall 4
        glBindTexture(GL_TEXTURE_2D, image2Texture);
        glDrawArrays(GL_TRIANGLES, 30, 6);

        //Draw wall 5
        glBindTexture(GL_TEXTURE_2D, wallTexture);
        glDrawArrays(GL_TRIANGLES, 36, 6);

        //Draw wall 6
        glBindTexture(GL_TEXTURE_2D, image3Texture);
        glDrawArrays(GL_TRIANGLES, 42, 6);

        //Draw wall 7
        glBindTexture(GL_TEXTURE_2D, wallTexture);
        glDrawArrays(GL_TRIANGLES, 48, 6);

        //Draw wall 8
        glBindTexture(GL_TEXTURE_2D, image4Texture);
        glDrawArrays(GL_TRIANGLES, 54, 6);

        //Draw wall 9
        glBindTexture(GL_TEXTURE_2D, wallTexture);
        glDrawArrays(GL_TRIANGLES, 60, 6);

        //Draw wall 10
        glBindTexture(GL_TEXTURE_2D, image5Texture);
        glDrawArrays(GL_TRIANGLES, 72, 6);

        // Draw wall 11
        glBindTexture(GL_TEXTURE_2D, wallTexture);
        glDrawArrays(GL_TRIANGLES, 78, 6);

        // Draw wall 12
        glBindTexture(GL_TEXTURE_2D, image6Texture);
        glDrawArrays(GL_TRIANGLES, 84, 6);

        //Draw wall 13
        glBindTexture(GL_TEXTURE_2D, wallTexture);
        glDrawArrays(GL_TRIANGLES, 90, 6);

        //Draw wall 14
        glBindTexture(GL_TEXTURE_2D, image7Texture);
        glDrawArrays(GL_TRIANGLES, 96, 6);

        //Draw wall 15
        glBindTexture(GL_TEXTURE_2D, wallTexture);
        glDrawArrays(GL_TRIANGLES, 102, 6);

        //Draw wall 16
        glBindTexture(GL_TEXTURE_2D, image8Texture);
        glDrawArrays(GL_TRIANGLES, 108, 6);

        //Draw wall 17
        glBindTexture(GL_TEXTURE_2D, wallTexture);
        glDrawArrays(GL_TRIANGLES, 114, 6);

        //Draw wall 18
        glBindTexture(GL_TEXTURE_2D, wallTexture);
        glDrawArrays(GL_TRIANGLES, 120, 6);

        //Draw wall 19
        glBindTexture(GL_TEXTURE_2D, wallTexture);
        glDrawArrays(GL_TRIANGLES, 126, 6);

        //Draw wall 20
        glBindTexture(GL_TEXTURE_2D, wallTexture);
        glDrawArrays(GL_TRIANGLES, 132, 30);

        //Draw wall 21
        glBindTexture(GL_TEXTURE_2D, wallTexture);
        glDrawArrays(GL_TRIANGLES, 162, 30);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteTextures(1, &floorTexture);
    glDeleteTextures(1, &wallTexture);

    glfwTerminate();
    return 0;
}
