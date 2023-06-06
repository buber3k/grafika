#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>
#include <vector>
#include <array>

#include "Shader.h"


struct Mesh {

    Mesh() {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);
    }
    ~Mesh() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
    }

    void passVertices(const std::vector<float>& vertices, const std::vector<uint32_t>& indices) {
        indicesCount = indices.size();
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), indices.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

    }

    void draw(Shader& shader) {
        glBindVertexArray(vao);
        shader.use();
        glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, nullptr);
    }
private:
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    GLuint indicesCount = 0;
};

struct Texture {
    Texture(const char* path) {
        stbi_set_flip_vertically_on_load(true);
        GLubyte* data = stbi_load(path, &width, &height, &nrChannels, 0);
        if (!data) {
            std::printf("Could not load!\n");
            return;
        }

        glGenTextures(1, &tid);
        glBindTexture(GL_TEXTURE_2D, tid);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        stbi_image_free(data);
    }
    ~Texture() {
		glDeleteTextures(1, &tid);
    }

    void use() {
        glBindTexture(GL_TEXTURE_2D, tid);
    }
private:
    GLuint tid;
    int width, height, nrChannels;
};

int renderedObjects = 0;
float intensity = 0.0;

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    printf("%f %f\n", xoffset, yoffset);
    intensity += yoffset * 0.02;
    if (intensity < 0.0) {
        intensity = 0.0;
    }
    if (intensity > 1.0) {
        intensity = 1.0;
    }
}

void processInputKeyboard(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        renderedObjects = 0;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        renderedObjects = 1;
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        renderedObjects = 2;
    }
}


int main()
{
    // inicjalizacja GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Tworzenie okna
    const unsigned int window_width = 900;
    const unsigned int window_height = 900;
    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Kwadrat Trojkat", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetScrollCallback(window, scroll_callback);


    // inicjalizacja GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    glViewport(0, 0, (GLuint)window_width, (GLuint)window_height);

    float offY = 0.7;
    Mesh triangle, square;
    triangle.passVertices(
        {-0.2, -0.2f + offY, 0.0,   0.0, 0.0,
          0.2, -0.2f + offY, 0.0,   0.0, 1.0,
          0.0,  0.2f + offY, 0.0,   1.0, 1.0,},
        {0, 1, 2}
    );
    square.passVertices(
        {-0.5,  0.5, 0.0,   0.0, 1.0,
         -0.5, -0.5, 0.0,   0.0, 0.0,
          0.5, -0.5, 0.0,   1.0, 0.0,
          0.5,  0.5, 0.0,   1.0, 1.0,},
        { 0, 1, 2,
          0, 2, 3 }
    );

    Texture tex0("first.png");
    Texture tex1("second.png");

    Shader shader("basic2.vert", "basic2.frag");
    glm::vec3 x{ 1.0f, 2.0f, 2.0f };

    // p�tla zdarzen
    while (!glfwWindowShouldClose(window))
    {
        // renderowanie
        glClearColor(0.298f, 0.141f, 0.141f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        shader.use();
        auto intensityUniform = glGetUniformLocation(shader(), "intensity");
        glUniform1f(intensityUniform, intensity);

        //auto colorUniform = glGetUniformLocation(shader(), "figureColor");
        //glUniform3fv(colorUniform, 1, colors[renderedObjects].data());
        //glBindVertexArray(vao);
        //glDrawElements(GL_TRIANGLES, figure.second.size(), GL_UNSIGNED_INT, nullptr);
        
        if (renderedObjects == 0 || renderedObjects == 2) {
            tex0.use();
            square.draw(shader);
        }
        if (renderedObjects == 1 || renderedObjects == 2) {
            tex1.use();
            triangle.draw(shader);
        }

        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR)
        {
            printf("err: %d\n", err);
        }

        processInputKeyboard(window);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glfwTerminate();
    return 0;
}
