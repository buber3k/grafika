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

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
}

void processInputKeyboard(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}


int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Tworzenie okna
    const unsigned int window_width = 900;
    const unsigned int window_height = 900;
    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Trojkaty", NULL, NULL);
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

    Mesh triangle;
    triangle.passVertices(
        {-0.1, -0.1f, 0.0,   0.0, 0.0,
          0.1, -0.1f, 0.0,   0.0, 1.0,
          0.0,  0.1f, 0.0,   1.0, 1.0,},
        {0, 1, 2}
    );

    Shader shader("basic3.vert", "basic3.frag");

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        double time = glfwGetTime();

        auto model = glm::mat4(1.0f);

        shader.use();
        auto colorUniform = glGetUniformLocation(shader(), "ColorTriangle");
        auto modelUniform = glGetUniformLocation(shader(), "Model");

        // 1# triangle
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3{
            -0.5f,
             0.5f,
             0.0f});
        model = glm::translate(model, glm::vec3{
             glm::sin(time) * 0.3f,
             glm::sin(time) * 0.3f,
             0.0f});
        glUniform3f(colorUniform, 1.0f, 0.0f, 0.0f);
        glUniformMatrix4fv(modelUniform, 1, GL_FALSE, glm::value_ptr(model));
		triangle.draw(shader);

        // 2# triangle
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3{
             0.5f,
             0.5f,
             0.0f});
        model = glm::rotate(model, (float)time, glm::vec3{0.0f, 0.0f, 1.0f});
        glUniform3f(colorUniform, 0.0f, 1.0f, 0.0f);
        glUniformMatrix4fv(modelUniform, 1, GL_FALSE, glm::value_ptr(model));
		triangle.draw(shader);

        // 3# triangle
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3{
             0.5f,
            -0.5f,
             0.0f});
        model = glm::scale(model, glm::vec3(glm::sin(time) * 0.5f + 0.8f));
        glUniform3f(colorUniform, 0.0f, 1.0f, 1.0f);
        glUniformMatrix4fv(modelUniform, 1, GL_FALSE, glm::value_ptr(model));
		triangle.draw(shader);
        
        // 4# triangle
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3{
            -0.5f,
            -0.5f,
             0.0f});
        model = glm::translate(model, glm::vec3{
             glm::sin(time) * 0.3f,
             glm::sin(time) * 0.3f,
             0.0f});
        model = glm::rotate(model, (float)time, glm::vec3{0.0f, 0.0f, 1.0f});
        model = glm::scale(model, glm::vec3(glm::cos(time) * 0.5f + 0.8f));
        glUniform3f(colorUniform, 0.0f, 0.0f, 1.0f);
        glUniformMatrix4fv(modelUniform, 1, GL_FALSE, glm::value_ptr(model));
		triangle.draw(shader);

        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR)
        {
            printf("err: %d\n", err);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glfwTerminate();
    return 0;
}
