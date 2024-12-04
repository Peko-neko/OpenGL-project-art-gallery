#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Vertex Shader source
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main() {
    gl_Position = vec4(aPos, 1.0);
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
    // Floor
    -0.5f,  0.0f, -0.5f,  0.0f, 0.0f,
     0.5f,  0.0f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.0f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.0f, -0.5f,  0.0f, 0.0f,
     0.5f,  0.0f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.0f,  0.5f,  0.0f, 1.0f,

    // Ceiling
    -0.5f,  1.0f, -0.5f,  0.0f, 0.0f,
     0.5f,  1.0f, -0.5f,  1.0f, 0.0f,
     0.5f,  1.0f,  0.5f,  1.0f, 1.0f,
    -0.5f,  1.0f, -0.5f,  0.0f, 0.0f,
     0.5f,  1.0f,  0.5f,  1.0f, 1.0f,
    -0.5f,  1.0f,  0.5f,  0.0f, 1.0f,

    // Walls
    -0.5f,  0.0f, -0.5f,  0.0f, 0.0f,
    -0.5f,  1.0f, -0.5f,  0.0f, 1.0f,
     0.5f,  1.0f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.0f, -0.5f,  0.0f, 0.0f,
     0.5f,  1.0f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.0f, -0.5f,  1.0f, 0.0f,

     0.5f,  0.0f, -0.5f,  0.0f, 0.0f,
     0.5f,  1.0f, -0.5f,  0.0f, 1.0f,
     0.5f,  1.0f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.0f, -0.5f,  0.0f, 0.0f,
     0.5f,  1.0f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.0f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.0f,  0.5f,  0.0f, 0.0f,
     0.5f,  1.0f,  0.5f,  0.0f, 1.0f,
    -0.5f,  1.0f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.0f,  0.5f,  0.0f, 0.0f,
    -0.5f,  1.0f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.0f,  0.5f,  1.0f, 0.0f,

    -0.5f,  0.0f,  0.5f,  0.0f, 0.0f,
    -0.5f,  1.0f,  0.5f,  0.0f, 1.0f,
    -0.5f,  1.0f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.0f,  0.5f,  0.0f, 0.0f,
    -0.5f,  1.0f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.0f, -0.5f,  1.0f, 0.0f
};

unsigned int loadTexture(const char* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;
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

    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO);

        // Draw floor and ceiling
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        glDrawArrays(GL_TRIANGLES, 0, 12);

        // Draw walls
        glBindTexture(GL_TEXTURE_2D, wallTexture);
        glDrawArrays(GL_TRIANGLES, 12, 24);

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
